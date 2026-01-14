#include "MacosOpenGlGlfwCanvas.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace arv
{
    MacosOpenGlGlfwCanvas::MacosOpenGlGlfwCanvas()
    {
    }

    MacosOpenGlGlfwCanvas::~MacosOpenGlGlfwCanvas()
    {
        Destroy();
    }

    void MacosOpenGlGlfwCanvas::Init()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        m_window = glfwCreateWindow(800, 600, "ARVision", nullptr, nullptr);
        if (!m_window)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_window);

        std::cout << "MacosOpenGlGlfwCanvas::Init() - Window created successfully" << std::endl;
    }

    void MacosOpenGlGlfwCanvas::PollEvents()
    {
        glfwPollEvents();
    }

    void MacosOpenGlGlfwCanvas::SwapBuffers()
    {
        if (m_window)
        {
            glfwSwapBuffers(m_window);
        }
    }

    void MacosOpenGlGlfwCanvas::Destroy()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        glfwTerminate();
    }

    bool MacosOpenGlGlfwCanvas::ShouldClose()
    {
        return m_window && glfwWindowShouldClose(m_window);
    }
}
