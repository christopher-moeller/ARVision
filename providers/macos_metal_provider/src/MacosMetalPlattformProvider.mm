#include "MacosMetalPlattformProvider.h"
#include "plattform/MacosMetalGlfwCanvas.h"
#include "rendering/MacosMetalRenderingAPI.h"
#include <iostream>

namespace arv
{
    MacosMetalPlattformProvider::MacosMetalPlattformProvider()
    {
        m_canvas = new MacosMetalGlfwCanvas();
        m_renderingAPI = new MacosMetalRenderingAPI();
    }

    MacosMetalPlattformProvider::~MacosMetalPlattformProvider()
    {
        delete m_canvas;
        delete m_renderingAPI;
    }

    void MacosMetalPlattformProvider::Init()
    {
        std::cout << "MacosMetalPlattformProvider::Init()" << std::endl;
        m_canvas->Init();

        // Pass the Metal layer from the canvas to the rendering API
        MacosMetalGlfwCanvas* metalCanvas = static_cast<MacosMetalGlfwCanvas*>(m_canvas);
        MacosMetalRenderingAPI* metalAPI = static_cast<MacosMetalRenderingAPI*>(m_renderingAPI);
        metalAPI->SetMetalLayer(metalCanvas->GetMetalLayer());

        m_renderingAPI->Init();
    }
}
