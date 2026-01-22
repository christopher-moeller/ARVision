#include <iostream>
#include <memory>
#include "ARVBase.h"
#include "MacosMetalPlatformProvider.h"
#include "MacosOpenGlPlatformProvider.h"
#include "platform/Canvas.h"
#include "layers/SceneLayer.h"

// Window configuration
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int main()
{
    // Toggle between Metal and OpenGL rendering backends
    bool useMetal = true;

    std::unique_ptr<arv::PlatformProvider> platformProvider;
    if(useMetal) {
        platformProvider = std::make_unique<arv::MacosMetalPlatformProvider>();
    } else {
        platformProvider = std::make_unique<arv::MacosOpenGlPlatformProvider>();
    }

    arv::ARVApplication* app = arv::ARVApplication::Create(platformProvider.get());
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

    while (!canvas->ShouldClose())
    {
        arv::Timestep timestep = app->CalculateNextTimestep();

        // Update all layers
        app->GetLayerStack().OnUpdate(timestep.GetSeconds());

        // Render all layers
        app->GetLayerStack().OnRender();

        // Step
        canvas->PollEvents();
        canvas->SwapBuffers();
    }

    arv::ARVApplication::Destroy();
    // platformProvider automatically cleaned up by unique_ptr

    return 0;
}
