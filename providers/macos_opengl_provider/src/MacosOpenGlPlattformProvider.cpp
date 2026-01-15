#include "MacosOpenGlPlattformProvider.h"
#include "plattform/MacosOpenGlGlfwCanvas.h"
#include "rendering/MacosOpenGlRenderingAPI.h"
#include <iostream>

namespace arv
{
    MacosOpenGlPlattformProvider::MacosOpenGlPlattformProvider()
    {
        m_canvas = new MacosOpenGlGlfwCanvas();
        m_renderingAPI = new MacosOpenGlRenderingAPI();
    }

    MacosOpenGlPlattformProvider::~MacosOpenGlPlattformProvider()
    {
        delete m_canvas;
        delete m_renderingAPI;
    }

    void MacosOpenGlPlattformProvider::Init()
    {
        std::cout << "MacosOpenGlPlattformProvider::Init()" << std::endl;
        m_canvas->Init();
        m_renderingAPI->Init();
    }
    
}
