#include "ARVApplication.h"
#include "ARVBase.h"
#include "rendering/Renderer.h"
#include <iostream>
#include <chrono>

#include "utils/StdLogger.h"
#include "events/CoreEventManager.h"
#include "platform/CorePlatformApplicationContext.h"

namespace arv
{

    ARVApplication* ARVApplication::s_Instance = nullptr;

    ARVApplication* ARVApplication::Get() {
        return s_Instance;
    }

    ARVApplication* ARVApplication::Create(std::unique_ptr<PlatformProvider> platformProvider) {
        s_Instance = new ARVApplication(std::move(platformProvider));
        s_Instance->GetLogger()->Info("ARVApplication::Create() - Application instance created");
        return s_Instance;
    }

    void ARVApplication::Destroy() {
        if (s_Instance) {
            s_Instance->GetLogger()->Info("ARVApplication::Destroy() - Destroying application instance");
        }
        delete s_Instance;
        s_Instance = nullptr;
    }

    ARVApplication::ARVApplication(std::unique_ptr<PlatformProvider> platformProvider)
        : m_platformProvider(std::move(platformProvider))
    {
        m_Logger = std::make_unique<StdLogger>();
        m_EventManager = std::make_unique<CoreEventManager>();
        m_Logger->Info("ARVApplication::ARVApplication() - Constructor called, Logger and EventManager initialized");
    }

    void ARVApplication::Initialize()
    {
        ARV_LOG_INFO("ARVApplication::Initialize() - Starting initialization");

        Logger* customLogger = m_platformProvider->CreateCustomLogger();
        if(customLogger) {
            ARV_LOG_INFO("ARVApplication::Initialize() - Custom logger provided by platform, switching logger");
            m_Logger.reset(customLogger);
        } else {
            ARV_LOG_INFO("ARVApplication::Initialize() - Using default StdLogger");
        }

        ARV_LOG_INFO("ARVApplication::Initialize() - Creating platform application context");
        CorePlatformApplicationContext context(m_Logger.get(), m_EventManager.get());

        ARV_LOG_INFO("ARVApplication::Initialize() - Initializing platform provider");
        m_platformProvider->Init(&context);

        ARV_LOG_INFO("ARVApplication::Initialize() - Creating renderer");
        m_renderer = std::make_unique<Renderer>(m_platformProvider->GetRenderingAPI());

        ARV_LOG_INFO("ARVApplication::Initialize() - Registering ApplicationResizeEvent listener");
        m_EventManager->AddListener(EventType::ApplicationResizeEvent, [this](arv::Event& event) {
            ApplicationResizeEvent* resizeEvent = static_cast<ApplicationResizeEvent*>(&event);
            this->m_Width = resizeEvent->GetWidth();
            this->m_Height = resizeEvent->GetHeight();
            ARV_LOG_INFO("ARVApplication - Window resized to {}x{}", this->m_Width, this->m_Height);

            this->GetRenderer()->OnTargetResize(this->m_Width, this->m_Height);
            return false;
        });

        ARV_LOG_INFO("ARVApplication::Initialize() - Initialization complete");
    }

    PlatformProvider* ARVApplication::GetPlatformProvider() const
    {
        return m_platformProvider.get();
    }

    Renderer* ARVApplication::GetRenderer() const
    {
        return m_renderer.get();
    }

    Timestep ARVApplication::CalculateNextTimestep() {
        float time = GetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;
        return timestep;
    }

    float ARVApplication::GetTime() {
        using namespace std::chrono;
        static auto start = steady_clock::now();
        auto now = steady_clock::now();
        return duration<float>(now - start).count();
    }

    void ARVApplication::PushLayer(std::unique_ptr<Layer> layer) {
        m_LayerStack.PushLayer(std::move(layer));
    }

    void ARVApplication::PushOverlay(std::unique_ptr<Layer> overlay) {
        m_LayerStack.PushOverlay(std::move(overlay));
    }
}
