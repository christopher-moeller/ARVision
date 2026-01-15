#include <iostream>
#include "ARVApplication.h"
#include "MacosMetalPlattformProvider.h"
#include "MacosOpenGlPlattformProvider.h"
#include "plattform/Canvas.h"
#include "rendering/RenderingAPI.h"
#include "objects/ExampleMetalTriangleRO.h"
#include "objects/ExampleTriangleRO.h"

int main()
{

    bool useMetal = true;
    
    arv::PlattformProvider* plattformProvider;
    if(useMetal) {
        plattformProvider = new arv::MacosMetalPlattformProvider();
    } else {
        plattformProvider = new arv::MacosOpenGlPlattformProvider();
    }

    arv::ARVApplication* app = arv::ARVApplication::Create(plattformProvider);

    app->Initialize();

    arv::Canvas* canvas = plattformProvider->GetCanvas();

    arv::RenderingObject* renderingObject;
    
    if(useMetal) {
        renderingObject = new arv::ExampleMetalTriangleRO();
    } else {
        renderingObject = new arv::ExampleTriangleRO();
    }

    while (!canvas->ShouldClose())
    {
        canvas->PollEvents();
        app->GetRenderer()->DrawObject(*renderingObject);
        
        canvas->SwapBuffers();
    }

    delete app;
    delete plattformProvider;

    return 0;
}
