#include "MacosMetalPlatformProvider.h"
#include "platform/MacosMetalGlfwCanvas.h"
#include "rendering/MacosMetalRenderingAPI.h"
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
        std::cout << "MacosMetalPlatformProvider::Init()" << std::endl;
        m_canvas->Init(context);

        // Pass the Metal layer from the canvas to the rendering API
        MacosMetalGlfwCanvas* metalCanvas = static_cast<MacosMetalGlfwCanvas*>(m_canvas.get());
        MacosMetalRenderingAPI* metalAPI = static_cast<MacosMetalRenderingAPI*>(m_renderingAPI.get());
        metalAPI->SetMetalLayer(metalCanvas->GetMetalLayer());

        m_renderingAPI->Init(context);
    }
}
