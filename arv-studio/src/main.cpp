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

int current_platform = PROVIDER_OPENGL;

void start(int platform_provider);

arv::ARVApplication* initializeApplication(int providerType);
void runApplication(arv::ARVApplication* app, bool &stopMainLoop);
void destroyApplication(arv::ARVApplication* app);

int main()
{
    while (current_platform) {
        start(current_platform);
    }
    return 0;
}

void start(int platform_provider) {
    arv::ARVApplication* app = initializeApplication(platform_provider);
    if(!app) {
        return;
    }
    
    bool stopApplication = false;
    app->GetEventManager()->AddListener(arv::EventType::CustomActionEvent, [&stopApplication](arv::Event& event) {
        arv::CustomActionEvent* actionEvent = static_cast<arv::CustomActionEvent*>(&event);
        arv::EventDataOnPlatformChange* data = static_cast<arv::EventDataOnPlatformChange*>(actionEvent->GetData());
        current_platform = data->changeTo;
        stopApplication = true;
        return false;
    });
    
    runApplication(app, stopApplication);

    destroyApplication(app);
}

arv::ARVApplication* initializeApplication(int providerType) {
    

    arv::PlatformProvider* platformProvider;
    if(providerType == PROVIDER_OPENGL) {
        platformProvider = new arv::MacosOpenGlPlatformProvider();
    } else if(providerType == PROVIDER_METAL) {
        platformProvider = new arv::MacosMetalPlatformProvider();
    } else {
        current_platform = 0;
        return nullptr;
        // TODO: ERROR: stop application
    }

    current_platform = providerType;
    arv::ARVApplication* app = arv::ARVApplication::Create(platformProvider);
    ARV_LOG_INFO("ARV Application created");

    app->Initialize();
    
    // Push the scene layer with configured window dimensions
    app->PushLayer(std::make_unique<SceneLayer>(
        app->GetRenderer(),
        app->GetEventManager().get(),
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    ));
    
    arv::Canvas* canvas = platformProvider->GetCanvas();
    
    // Push ImGui layer as an overlay (renders on top of scene)
    app->PushOverlay(std::make_unique<ImGuiLayer>(
        canvas,
        platformProvider->GetRenderingAPI()
    ));
    
    return app;
}

void runApplication(arv::ARVApplication* app, bool &stopMainLoop) {
    
    arv::PlatformProvider* platformProvider = app->GetPlatformProvider();
    arv::Canvas* canvas = platformProvider->GetCanvas();

    arv::RenderingAPI* renderingAPI = platformProvider->GetRenderingAPI();

    while (!canvas->ShouldClose() && !stopMainLoop)
    {
        arv::Timestep timestep = app->CalculateNextTimestep();

        // Update all layers
        app->GetLayerStack().OnUpdate(timestep.GetSeconds());

        // Begin frame before rendering
        renderingAPI->BeginFrame();

        // Render all layers (scene + ImGui)
        app->GetLayerStack().OnRender();

        // End frame after all layers have rendered
        renderingAPI->EndFrame();

        // Step
        canvas->PollEvents();
        canvas->SwapBuffers();
    }
    
    if(canvas->ShouldClose()) {
        current_platform = 0;
    }
    
}

void destroyApplication(arv::ARVApplication* app) {
    arv::PlatformProvider* platformProvider = app->GetPlatformProvider();
    arv::ARVApplication::Destroy();
    delete platformProvider; // TODO should be in Application
}
