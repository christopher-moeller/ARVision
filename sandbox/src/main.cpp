#include <iostream>
#include "ARVApplication.h"
#include "TestCanvas.h"

int main()
{
    arv::TestCanvas* testCanvas = new arv::TestCanvas();
    
    arv::ARVCoreConfiguration* coreConfiguration = new arv::ARVCoreConfiguration(testCanvas);
    arv::ARVApplication* app = new arv::ARVApplication(coreConfiguration);
    
    app->Initialize();
    
    delete app;
    delete coreConfiguration;
    
    return 0;
}
