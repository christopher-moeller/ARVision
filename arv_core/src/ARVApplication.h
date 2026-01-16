#pragma once

#include <memory>
#include "utils/Logger.h"
#include "events/EventManager.h"

#include "PlattformProvider.h"
#include "rendering/Renderer.h"
#include "utils/Timestep.h"

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
        inline std::unique_ptr<EventManager>& GetEventManager() { return m_EventManager; }
        
        inline int GetWidth() { return m_Width; }
        inline int GetHeight() { return m_Height; }
        
        float GetTime();
        Timestep CalculateNextTimestep();

    protected:
        ARVApplication(PlattformProvider* plattformProvider);
        
        std::unique_ptr<Logger> m_Logger;
        std::unique_ptr<EventManager> m_EventManager;

        int m_Width, m_Height;

    private:
        PlattformProvider* m_plattformProvider;
        Renderer* m_renderer;
        
        float m_LastFrameTime = 0.0f;

        static ARVApplication* s_Instance;
    };
}
