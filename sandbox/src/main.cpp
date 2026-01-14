#include <iostream>
#include "ARVApplication.h"
#include "MacosPlattformProvider.h"

int main()
{
    arv::MacosPlattformProvider* plattformProvider = new arv::MacosPlattformProvider();
    arv::ARVApplication* app = new arv::ARVApplication(plattformProvider);
    
    app->Initialize();
    
    delete app;
    
    return 0;
}
