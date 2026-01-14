#include "ARVApplication.h"
#include "rendering/Renderer.h"
#include <iostream>

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
    }

    void ARVApplication::Initialize()
    {
        m_plattformProvider->Init();
        m_renderer = new Renderer(m_plattformProvider->GetRenderingAPI());
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
