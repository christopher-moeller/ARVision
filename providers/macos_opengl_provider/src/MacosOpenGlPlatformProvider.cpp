#include "MacosOpenGlPlatformProvider.h"
#include "platform/MacosOpenGlGlfwCanvas.h"
#include "rendering/MacosOpenGlRenderingAPI.h"
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
        std::cout << "MacosOpenGlPlatformProvider::Init()" << std::endl;
        m_canvas->Init(context);
        m_renderingAPI->Init(context);
    }

}
