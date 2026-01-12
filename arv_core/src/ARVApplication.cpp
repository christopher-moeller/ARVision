#include "ARVApplication.h"
#include <iostream>


namespace arv
{
    ARVApplication::ARVApplication(ARVCoreProvider& coreProvider)
        : m_coreProvider(coreProvider)
    {
    }

    void ARVApplication::Initialize()
    {
        std::cout << "ARVApplication Initialized." << std::endl;
        std::cout << m_coreProvider.get_version() << std::endl;
    }
}
