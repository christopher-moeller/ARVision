#include "ARVApplication.h"
#include "rendering/Renderer.h"
#include <iostream>
#include <chrono>

#include "utils/StdLogger.h"
#include "events/CoreEventManager.h"
#include "plattform/CorePlattformApplicationContext.h"

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
        Logger* customLogger = m_plattformProvider->CreateCustomLogger();
        if(customLogger) {
            m_Logger.reset(customLogger);
        }

        CorePlattformApplicationContext context(m_Logger.get(), m_EventManager.get());
        m_plattformProvider->Init(&context);
        m_renderer = std::make_unique<Renderer>(m_plattformProvider->GetRenderingAPI());

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
