#include <iostream>
#include <cmath>
#include <chrono>
#include "ARVBase.h"
#include "MacosMetalPlattformProvider.h"
#include "MacosOpenGlPlattformProvider.h"
#include "plattform/Canvas.h"
#include "rendering/RenderingAPI.h"
#include "objects/ExampleTriangleRO.h"
#include "objects/ImageTextureRO.h"
#include <glm/glm.hpp>
#include "camera/StandardCamera.h"
#include "camera/StandardCameraController.h"

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
    
    arv::StandardCamera* standardCamera = new arv::StandardCamera(800, 600);

    arv::StandardCameraController cameraController(standardCamera, arv::DeviceType::DESKTOP_COMPUTER);
    cameraController.Init();

    arv::Canvas* canvas = plattformProvider->GetCanvas();

    arv::ExampleTriangleRO* triangleObject = new arv::ExampleTriangleRO();
    arv::ImageTextureRO* imageObject = new arv::ImageTextureRO("/Users/cmoeller/dev/projects/ARVision/arv-studio/assets/fc-logo.png");

    // Position objects side by side
    triangleObject->SetPosition(glm::vec3(-1.0f, 0.0f, 0.0f));
    imageObject->SetPosition(glm::vec3(1.0f, 0.0f, 0.0f));

    auto startTime = std::chrono::high_resolution_clock::now();

    while (!canvas->ShouldClose())
    {

        // Animate the color over time (pulsing effect)
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(currentTime - startTime).count();
        float r = (std::sin(time * 2.0f) + 1.0f) / 2.0f;
        float g = (std::sin(time * 2.0f + 2.0f) + 1.0f) / 2.0f;
        float b = (std::sin(time * 2.0f + 4.0f) + 1.0f) / 2.0f;
        triangleObject->SetColor(glm::vec4(r, g, b, 1.0f));

        arv::Timestep timestep = app->CalculateNextTimestep();
        arv::CameraControllerAppContext context(app->GetEventManager().get(), timestep);
        cameraController.UpdateOnStep(context);

        arv::Scene scene = app->GetRenderer()->NewScene(standardCamera);
        scene.ClearColor({0.2f, 0.3f, 0.3f, 1.0f});
        scene.Submit(*triangleObject);
        scene.Submit(*imageObject, imageObject->GetTexture());
        scene.Render();

        // Step
        canvas->PollEvents();
        canvas->SwapBuffers();
    }

    delete imageObject;
    delete triangleObject;
    delete app;
    delete plattformProvider;

    return 0;
}
