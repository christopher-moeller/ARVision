#include "MacosOpenGlPlattformProvider.h"
#include "plattform/MacosOpenGlGlfwCanvas.h"
#include "rendering/MacosOpenGlRenderingAPI.h"
#include <iostream>
#include <memory>

namespace arv
{
    MacosOpenGlPlattformProvider::MacosOpenGlPlattformProvider()
    {
        m_canvas = std::make_unique<MacosOpenGlGlfwCanvas>();
        m_renderingAPI = std::make_unique<MacosOpenGlRenderingAPI>();
    }

    MacosOpenGlPlattformProvider::~MacosOpenGlPlattformProvider() = default;

    void MacosOpenGlPlattformProvider::Init(PlattformApplicationContext* context)
    {
        std::cout << "MacosOpenGlPlattformProvider::Init()" << std::endl;
        m_canvas->Init(context);
        m_renderingAPI->Init(context);
    }

}
