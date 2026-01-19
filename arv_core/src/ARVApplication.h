#pragma once

#include <memory>
#include "utils/Logger.h"
#include "events/EventManager.h"
#include "LayerStack.h"

#include "PlatformProvider.h"
#include "rendering/Renderer.h"
#include "utils/Timestep.h"

namespace arv
{

    class ARVApplication
    {
    public:
        static ARVApplication* Create(PlatformProvider* platformProvider);
        static void Destroy();
        static ARVApplication* Get();
        ARVApplication(ARVApplication& obj) = delete;

        void Initialize();
        PlatformProvider* GetPlatformProvider() const;
        Renderer* GetRenderer() const;

        inline std::unique_ptr<Logger>& GetLogger() { return m_Logger; }
        inline std::unique_ptr<EventManager>& GetEventManager() { return m_EventManager; }

        void PushLayer(std::unique_ptr<Layer> layer);
        void PushOverlay(std::unique_ptr<Layer> overlay);
        LayerStack& GetLayerStack() { return m_LayerStack; }

        inline int GetWidth() { return m_Width; }
        inline int GetHeight() { return m_Height; }
        
        float GetTime();
        Timestep CalculateNextTimestep();

    protected:
        ARVApplication(PlatformProvider* platformProvider);
        
        std::unique_ptr<Logger> m_Logger;
        std::unique_ptr<EventManager> m_EventManager;

        int m_Width, m_Height;

    private:
        PlatformProvider* m_platformProvider;
        std::unique_ptr<Renderer> m_renderer;
        LayerStack m_LayerStack;

        float m_LastFrameTime = 0.0f;

        static ARVApplication* s_Instance;
    };
}
