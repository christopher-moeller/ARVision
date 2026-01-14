#pragma once

#include "plattform/PlattformProvider.h"

namespace arv
{
    class ARVApplication
    {
    public:
        explicit ARVApplication(PlattformProvider* plattformProvider);
        void Initialize();
        PlattformProvider* GetPlattformProvider() const;

    private:
        PlattformProvider* m_plattformProvider;
    };
}
