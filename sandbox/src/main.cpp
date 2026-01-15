#include <iostream>
#include "ARVApplication.h"
#include "MacosMetalPlattformProvider.h"
#include "plattform/Canvas.h"
#include "rendering/RenderingAPI.h"
#include "objects/ExampleMetalTriangleRO.h"

int main()
{

    arv::MacosMetalPlattformProvider* plattformProvider = new arv::MacosMetalPlattformProvider();
    arv::ARVApplication* app = arv::ARVApplication::Create(plattformProvider);

    app->Initialize();

    arv::Canvas* canvas = plattformProvider->GetCanvas();

    arv::ExampleMetalTriangleRO triangle;

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
