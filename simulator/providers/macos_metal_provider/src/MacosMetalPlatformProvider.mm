#include "MacosMetalPlatformProvider.h"
#include "platform/MacosMetalGlfwCanvas.h"
#include "rendering/MacosMetalRenderingAPI.h"
#include "ARVBase.h"
#include <iostream>
#include <memory>

namespace arv
{
    MacosMetalPlatformProvider::MacosMetalPlatformProvider()
    {
        m_canvas = std::make_unique<MacosMetalGlfwCanvas>();
        m_renderingAPI = std::make_unique<MacosMetalRenderingAPI>();
    }

    MacosMetalPlatformProvider::~MacosMetalPlatformProvider() = default;

    void MacosMetalPlatformProvider::Init(PlatformApplicationContext* context)
    {
        ARV_LOG_INFO("MacosMetalPlatformProvider::Init() - Initializing Metal platform provider");
        ARV_LOG_INFO("MacosMetalPlatformProvider::Init() - Initializing canvas");
        m_canvas->Init(context);

        ARV_LOG_INFO("MacosMetalPlatformProvider::Init() - Connecting Metal layer to rendering API");
        MacosMetalGlfwCanvas* metalCanvas = static_cast<MacosMetalGlfwCanvas*>(m_canvas.get());
        MacosMetalRenderingAPI* metalAPI = static_cast<MacosMetalRenderingAPI*>(m_renderingAPI.get());
        metalAPI->SetMetalLayer(metalCanvas->GetMetalLayer());

        ARV_LOG_INFO("MacosMetalPlatformProvider::Init() - Initializing rendering API");
        m_renderingAPI->Init(context);
        ARV_LOG_INFO("MacosMetalPlatformProvider::Init() - Metal platform provider initialized");
    }
}
