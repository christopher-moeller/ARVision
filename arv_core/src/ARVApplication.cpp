#include "ARVApplication.h"
#include <iostream>


namespace arv
{
    ARVApplication::ARVApplication(PlattformProvider* plattformProvider)
        : m_plattformProvider(plattformProvider)
    {
    }

    void ARVApplication::Initialize()
    {
        this->m_plattformProvider->Init();
        std::cout << "ARVApplication Initialized." << std::endl;
    }

    PlattformProvider* ARVApplication::GetPlattformProvider() const
    {
        return m_plattformProvider;
    }
}
