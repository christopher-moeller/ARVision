#include <iostream>
#include <memory>
#include "ARVBase.h"
#include "MacosMetalPlatformProvider.h"
#include "MacosOpenGlPlatformProvider.h"
#include "platform/Canvas.h"
#include "rendering/RenderingAPI.h"
#include "layers/SceneLayer.h"
#include "layers/ImGuiLayer.h"
#include "events/StudioActionEvents.h"

// Window configuration
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

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
    app->Initialize();

    // Push the scene layer
    app->PushLayer(std::make_unique<SceneLayer>(
        app->GetRenderer(),
        app->GetEventManager().get(),
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    ));

    // Push ImGui layer as an overlay
    arv::PlatformProvider* provider = app->GetPlatformProvider();
    app->PushOverlay(std::make_unique<ImGuiLayer>(
        provider->GetCanvas(),
        provider->GetRenderingAPI()
    ));

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
        arv::Timestep timestep = app->CalculateNextTimestep();
        app->GetLayerStack().OnUpdate(timestep.GetSeconds());

        renderingAPI->BeginFrame();
        app->GetLayerStack().OnRender();
        renderingAPI->EndFrame();

        canvas->PollEvents();
        canvas->SwapBuffers();
    }

    // Determine what to return
    bool windowClosed = canvas->ShouldClose();
    arv::ARVApplication::Destroy();

    return windowClosed ? 0 : nextPlatform;
}
