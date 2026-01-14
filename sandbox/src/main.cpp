#include <iostream>
#include "ARVApplication.h"
#include "plattform/MacosPlattformProvider.h"
#include "plattform/Canvas.h"
#include "rendering/RenderingAPI.h"

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
        app->GetRenderer()->Draw();
    
        canvas->SwapBuffers();
    }

    delete app;

    return 0;
}
