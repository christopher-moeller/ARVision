#pragma once

#include "PlattformProvider.h"
#include "rendering/Renderer.h"

namespace arv
{

    class ARVApplication
    {
    public:
        explicit ARVApplication(PlattformProvider* plattformProvider);
        void Initialize();
        PlattformProvider* GetPlattformProvider() const;
        Renderer* GetRenderer() const;

    private:
        PlattformProvider* m_plattformProvider;
        Renderer* m_renderer;
    };
}
