#pragma once

#include "PlattformProvider.h"
#include "rendering/Renderer.h"

namespace arv
{

    class ARVApplication
    {
    public:
        static ARVApplication* Create(PlattformProvider* plattformProvider);
        static void Destroy();
        static ARVApplication* Get();
        ARVApplication(ARVApplication& obj) = delete;

        void Initialize();
        PlattformProvider* GetPlattformProvider() const;
        Renderer* GetRenderer() const;

    protected:
        ARVApplication(PlattformProvider* plattformProvider);

    private:
        PlattformProvider* m_plattformProvider;
        Renderer* m_renderer;

        static ARVApplication* s_Instance;
    };
}
