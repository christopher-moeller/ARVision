#include <iostream>
#include "ARVApplication.h"
#include "MacosOpenGlPlattformProvider.h"
#include "plattform/Canvas.h"
#include "rendering/RenderingAPI.h"
#include "objects/ExampleTriangleRO.h"

int main()
{
    
    arv::MacosOpenGlPlattformProvider* plattformProvider = new arv::MacosOpenGlPlattformProvider();
    arv::ARVApplication* app = arv::ARVApplication::Create(plattformProvider);
    
    app->Initialize();

    arv::Canvas* canvas = plattformProvider->GetCanvas();
    arv::RenderingAPI* renderingAPI = plattformProvider->GetRenderingAPI();

    renderingAPI->Init();
    
    arv::ExampleTriangleRO triangle;

    while (!canvas->ShouldClose())
    {
        canvas->PollEvents();
        app->GetRenderer()->DrawObject(triangle);
        canvas->SwapBuffers();
    }

    delete app;
    delete plattformProvider;

    return 0;
}
