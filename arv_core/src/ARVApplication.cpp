#include "ARVApplication.h"
#include <iostream>


namespace arv
{
    ARVApplication::ARVApplication(ARVCoreConfiguration* coreConfiguration)
        : m_coreConfiguration(coreConfiguration)
    {
    }

    void ARVApplication::Initialize()
    {
        std::cout << "ARVApplication Initialized." << std::endl;
        m_coreConfiguration->GetPlattformCanvas()->InitializeCanvas();
    }
}
