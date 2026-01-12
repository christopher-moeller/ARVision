#pragma once

#include "plattform/PlattformCanvas.h"

namespace arv
{
    class ARVCoreConfiguration
    {
    public:
        explicit ARVCoreConfiguration(PlattformCanvas* plattformCanvas);

        inline PlattformCanvas* GetPlattformCanvas() { return m_plattformCanvas; };
    private:
        PlattformCanvas* m_plattformCanvas;
    };
}
