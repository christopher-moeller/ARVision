#include <iostream>
#include "ARVApplication.h"
#include "plattform/MacosPlattformProvider.h"
#include "plattform/Canvas.h"
#include "rendering/RenderingAPI.h"
#include <glm/glm.hpp>

int main()
{
    
    arv::MacosPlattformProvider* plattformProvider = new arv::MacosPlattformProvider();
    arv::ARVApplication* app = new arv::ARVApplication(plattformProvider);

    app->Initialize();

    arv::Canvas* canvas = plattformProvider->GetCanvas();
    arv::RenderingAPI* renderingAPI = plattformProvider->GetRenderingAPI();

    while (!canvas->ShouldClose())
    {
        canvas->PollEvents();
        renderingAPI->SetClearColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        renderingAPI->Clear();
        canvas->SwapBuffers();
    }

    delete app;

    return 0;
}
