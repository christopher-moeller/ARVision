#include <iostream>
#include "ARVApplication.h"
#include "plattform/MacosPlattformProvider.h"
#include "plattform/Canvas.h"

int main()
{
    arv::MacosPlattformProvider* plattformProvider = new arv::MacosPlattformProvider();
    arv::ARVApplication* app = new arv::ARVApplication(plattformProvider);

    app->Initialize();

    arv::Canvas* canvas = plattformProvider->GetCanvas();

    while (!canvas->ShouldClose())
    {
        canvas->PollEvents();
        canvas->SwapBuffers();
    }

    delete app;

    return 0;
}
