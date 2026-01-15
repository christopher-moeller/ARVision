#include "ARVApplication.h"
#include "rendering/Renderer.h"
#include <iostream>

#include "utils/StdLogger.h"
#include "events/CoreEventManager.h"

namespace arv
{

    ARVApplication* ARVApplication::s_Instance = nullptr;

    ARVApplication* ARVApplication::Get() {
        return s_Instance;
    }

    ARVApplication* ARVApplication::Create(PlattformProvider* plattformProvider) {
        s_Instance = new ARVApplication(plattformProvider);
        return s_Instance;
    }

    void ARVApplication::Destroy() {
        delete s_Instance;
    }

    ARVApplication::ARVApplication(PlattformProvider* plattformProvider)
        : m_plattformProvider(plattformProvider)
    {
        m_Logger = std::make_unique<StdLogger>();
        m_EventManager = std::make_unique<CoreEventManager>();
    }

    void ARVApplication::Initialize()
    {
        m_plattformProvider->Init();
        m_renderer = new Renderer(m_plattformProvider->GetRenderingAPI());

        Logger* customLogger = m_plattformProvider->CreateCustomLogger();
        if(customLogger) {
            m_Logger.reset(customLogger);
        }

        m_EventManager->AddListener(EventType::ApplicationResizeEvent, [this](arv::Event& event) {
            ApplicationResizeEvent* resizeEvent = static_cast<ApplicationResizeEvent*>(&event);
            this->m_Width = resizeEvent->GetWidth();
            this->m_Height = resizeEvent->GetHeight();
            
            this->GetRenderer()->OnTargetResize(this->m_Width, this->m_Height);
            return false;
        });
        
        std::cout << "ARVApplication Initialized." << std::endl;
    }

    PlattformProvider* ARVApplication::GetPlattformProvider() const
    {
        return m_plattformProvider;
    }

    Renderer* ARVApplication::GetRenderer() const
    {
        return m_renderer;
    }
}
