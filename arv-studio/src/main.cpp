#include <iostream>
#include "ARVBase.h"
#include "MacosMetalPlattformProvider.h"
#include "MacosOpenGlPlattformProvider.h"
#include "plattform/Canvas.h"
#include "layers/SceneLayer.h"

int main()
{
    // Toggle between Metal and OpenGL rendering backends
    bool useMetal = true;

    arv::PlattformProvider* plattformProvider;
    if(useMetal) {
        plattformProvider = new arv::MacosMetalPlattformProvider();
    } else {
        plattformProvider = new arv::MacosOpenGlPlattformProvider();
    }

    arv::ARVApplication* app = arv::ARVApplication::Create(plattformProvider);
    ARV_LOG_INFO("ARV Application created");

    app->Initialize();

    // Push the scene layer
    app->PushLayer(std::make_unique<SceneLayer>(
        app->GetRenderer(),
        app->GetEventManager().get()
    ));

    arv::Canvas* canvas = plattformProvider->GetCanvas();

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
    delete plattformProvider;

    return 0;
}
