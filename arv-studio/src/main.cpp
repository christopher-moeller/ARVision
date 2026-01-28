#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "ARVBase.h"
#include "MacosMetalPlatformProvider.h"
#include "MacosOpenGlPlatformProvider.h"
#include "platform/Canvas.h"
#include "rendering/RenderingAPI.h"
#include "layers/MainLayer.h"
#include "events/StudioActionEvents.h"
#include "RenderingObjectRegistration.h"

// Window configuration
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;

// Returns the next platform to use, or 0 to exit
int start(int platformType);

int main()
{

    int platform = PROVIDER_OPENGL;
    while (platform) {
        platform = start(platform);
    }
    return 0;
}

int start(int platformType) {
    std::unique_ptr<arv::PlatformProvider> platformProvider;
    if (platformType == PROVIDER_OPENGL) {
        platformProvider = std::make_unique<arv::MacosOpenGlPlatformProvider>();
    } else if (platformType == PROVIDER_METAL) {
        platformProvider = std::make_unique<arv::MacosMetalPlatformProvider>();
    } else {
        return 0;
    }

    arv::ARVApplication* app = arv::ARVApplication::Create(std::move(platformProvider));
    if (!app) {
        return 0;
    }
    ARV_LOG_INFO("ARV Application created");
    
    // Register all RenderingObject factories before loading any scenes
    arv::RegisterRenderingObjects();
    
    app->Initialize();

    // Push the main layer (combines scene rendering and ImGui)
    arv::PlatformProvider* provider = app->GetPlatformProvider();
    auto mainLayer = std::make_unique<MainLayer>(
        app->GetRenderer(),
        provider->GetCanvas(),
        provider->GetRenderingAPI(),
        app->GetEventManager().get(),
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );
    MainLayer* mainLayerPtr = mainLayer.get();
    app->PushLayer(std::move(mainLayer));

    // Track platform switch requests
    int nextPlatform = 0;
    bool stopApplication = false;
    app->GetEventManager()->AddListener(arv::EventType::CustomActionEvent,
        [&nextPlatform, &stopApplication](arv::Event& event) {
            auto* actionEvent = static_cast<arv::CustomActionEvent*>(&event);
            auto* data = static_cast<arv::EventDataOnPlatformChange*>(actionEvent->GetData());
            nextPlatform = data->changeTo;
            stopApplication = true;
            return false;
        });

    // Run the application loop
    arv::Canvas* canvas = provider->GetCanvas();
    arv::RenderingAPI* renderingAPI = provider->GetRenderingAPI();

    while (!canvas->ShouldClose() && !stopApplication) {
        auto frameStart = std::chrono::steady_clock::now();

        arv::Timestep timestep = app->CalculateNextTimestep();
        app->GetLayerStack().OnUpdate(timestep.GetSeconds());

        renderingAPI->BeginFrame();
        app->GetLayerStack().OnRender();
        renderingAPI->EndFrame();

        canvas->PollEvents();
        canvas->SwapBuffers();

        // FPS cap
        int maxFPS = mainLayerPtr->GetState().maxFPS;
        if (maxFPS > 0) {
            auto frameEnd = std::chrono::steady_clock::now();
            auto elapsed = frameEnd - frameStart;
            auto targetDuration = std::chrono::duration<double>(1.0 / maxFPS);
            if (elapsed < targetDuration) {
                std::this_thread::sleep_for(targetDuration - elapsed);
            }
        }
    }

    // Determine what to return
    bool windowClosed = canvas->ShouldClose();
    arv::ARVApplication::Destroy();

    return windowClosed ? 0 : nextPlatform;
}
