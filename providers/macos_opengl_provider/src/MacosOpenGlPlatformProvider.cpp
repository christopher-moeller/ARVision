#include "MacosOpenGlPlatformProvider.h"
#include "platform/MacosOpenGlGlfwCanvas.h"
#include "rendering/MacosOpenGlRenderingAPI.h"
#include "ARVBase.h"
#include <iostream>
#include <memory>

namespace arv
{
    MacosOpenGlPlatformProvider::MacosOpenGlPlatformProvider()
    {
        m_canvas = std::make_unique<MacosOpenGlGlfwCanvas>();
        m_renderingAPI = std::make_unique<MacosOpenGlRenderingAPI>();
    }

    MacosOpenGlPlatformProvider::~MacosOpenGlPlatformProvider() = default;

    void MacosOpenGlPlatformProvider::Init(PlatformApplicationContext* context)
    {
        ARV_LOG_INFO("MacosOpenGlPlatformProvider::Init() - Initializing OpenGL platform provider");
        ARV_LOG_INFO("MacosOpenGlPlatformProvider::Init() - Initializing canvas");
        m_canvas->Init(context);
        ARV_LOG_INFO("MacosOpenGlPlatformProvider::Init() - Initializing rendering API");
        m_renderingAPI->Init(context);
        ARV_LOG_INFO("MacosOpenGlPlatformProvider::Init() - OpenGL platform provider initialized");
    }

}
