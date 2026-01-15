#include "MacosPlattformProvider.h"
#include "plattform/MacosOpenGlGlfwCanvas.h"
#include "rendering/MacosOpenGlRenderingAPI.h"
#include <iostream>

namespace arv
{
    MacosPlattformProvider::MacosPlattformProvider()
    {
        m_canvas = new MacosOpenGlGlfwCanvas();
        m_renderingAPI = new MacosOpenGlRenderingAPI();
    }

    MacosPlattformProvider::~MacosPlattformProvider()
    {
        delete m_canvas;
        delete m_renderingAPI;
    }

    void MacosPlattformProvider::Init()
    {
        std::cout << "MacosPlattformProvider::Init()" << std::endl;
        m_canvas->Init();
        m_renderingAPI->Init();
    }
}
