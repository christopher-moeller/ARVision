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

    renderingAPI->Init();

    while (!canvas->ShouldClose())
    {
        canvas->PollEvents();
        renderingAPI->SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
        renderingAPI->Clear();
        renderingAPI->Draw();
        canvas->SwapBuffers();
    }

    delete app;

    return 0;
}
