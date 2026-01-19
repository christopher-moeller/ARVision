#include "MacosMetalPlattformProvider.h"
#include "plattform/MacosMetalGlfwCanvas.h"
#include "rendering/MacosMetalRenderingAPI.h"
#include <iostream>
#include <memory>

namespace arv
{
    MacosMetalPlattformProvider::MacosMetalPlattformProvider()
    {
        m_canvas = std::make_unique<MacosMetalGlfwCanvas>();
        m_renderingAPI = std::make_unique<MacosMetalRenderingAPI>();
    }

    MacosMetalPlattformProvider::~MacosMetalPlattformProvider() = default;

    void MacosMetalPlattformProvider::Init(PlattformApplicationContext* context)
    {
        std::cout << "MacosMetalPlattformProvider::Init()" << std::endl;
        m_canvas->Init(context);

        // Pass the Metal layer from the canvas to the rendering API
        MacosMetalGlfwCanvas* metalCanvas = static_cast<MacosMetalGlfwCanvas*>(m_canvas.get());
        MacosMetalRenderingAPI* metalAPI = static_cast<MacosMetalRenderingAPI*>(m_renderingAPI.get());
        metalAPI->SetMetalLayer(metalCanvas->GetMetalLayer());

        m_renderingAPI->Init(context);
    }
}
