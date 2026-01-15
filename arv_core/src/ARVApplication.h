#pragma once

#include <memory>
#include "utils/Logger.h"

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

        inline std::unique_ptr<Logger>& GetLogger() { return m_Logger; }

    protected:
        ARVApplication(PlattformProvider* plattformProvider);
        
        std::unique_ptr<Logger> m_Logger;

    private:
        PlattformProvider* m_plattformProvider;
        Renderer* m_renderer;

        static ARVApplication* s_Instance;
    };
}
