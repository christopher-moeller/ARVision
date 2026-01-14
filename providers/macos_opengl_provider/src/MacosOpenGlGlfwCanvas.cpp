#include "MacosOpenGlGlfwCanvas.h"
#include <iostream>

namespace arv
{
    MacosOpenGlGlfwCanvas::MacosOpenGlGlfwCanvas()
    {
    }

    MacosOpenGlGlfwCanvas::~MacosOpenGlGlfwCanvas()
    {
    }

    void MacosOpenGlGlfwCanvas::Init()
    {
        std::cout << "MacosOpenGlGlfwCanvas::Init()" << std::endl;
    }

    void MacosOpenGlGlfwCanvas::PollEvents()
    {
        std::cout << "MacosOpenGlGlfwCanvas::PollEvents()" << std::endl;
    }

    void MacosOpenGlGlfwCanvas::SwapBuffers()
    {
        std::cout << "MacosOpenGlGlfwCanvas::SwapBuffers()" << std::endl;
    }

    void MacosOpenGlGlfwCanvas::Destroy()
    {
        std::cout << "MacosOpenGlGlfwCanvas::Destroy()" << std::endl;
    }
}
