#include <iostream>
#include <cmath>
#include <chrono>
#include "ARVBase.h"
#include "MacosMetalPlattformProvider.h"
#include "MacosOpenGlPlattformProvider.h"
#include "plattform/Canvas.h"
#include "rendering/RenderingAPI.h"
#include "objects/ExampleTriangleRO.h"
#include <glm/glm.hpp>

int main()
{
    // Toggle between Metal and OpenGL rendering backends
    bool useMetal = false;

    arv::PlattformProvider* plattformProvider;
    if(useMetal) {
        plattformProvider = new arv::MacosMetalPlattformProvider();
    } else {
        plattformProvider = new arv::MacosOpenGlPlattformProvider();
    }

    arv::ARVApplication* app = arv::ARVApplication::Create(plattformProvider);
    ARV_LOG_INFO("ARV Application created");

    app->Initialize();

    arv::Canvas* canvas = plattformProvider->GetCanvas();

    arv::ExampleTriangleRO* renderingObject = new arv::ExampleTriangleRO();

    renderingObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    auto startTime = std::chrono::high_resolution_clock::now();

    while (!canvas->ShouldClose())
    {
        canvas->PollEvents();

        // Animate the color over time (pulsing effect)
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(currentTime - startTime).count();
        float r = (std::sin(time * 2.0f) + 1.0f) / 2.0f;
        float g = (std::sin(time * 2.0f + 2.0f) + 1.0f) / 2.0f;
        float b = (std::sin(time * 2.0f + 4.0f) + 1.0f) / 2.0f;
        renderingObject->SetColor(glm::vec4(r, g, b, 1.0f));

        app->GetRenderer()->DrawObject(*renderingObject);

        canvas->SwapBuffers();
    }

    delete renderingObject;
    delete app;
    delete plattformProvider;

    return 0;
}
