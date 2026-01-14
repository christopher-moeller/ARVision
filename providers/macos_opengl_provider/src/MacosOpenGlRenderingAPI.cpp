#include "MacosOpenGlRenderingAPI.h"
#include <iostream>
#include <GLFW/glfw3.h>

namespace arv
{
    MacosOpenGlRenderingAPI::MacosOpenGlRenderingAPI()
    {
    }

    MacosOpenGlRenderingAPI::~MacosOpenGlRenderingAPI()
    {
    }

    void MacosOpenGlRenderingAPI::Init()
    {
        std::cout << "MacosOpenGlRenderingAPI::Init()" << std::endl;
    }

    void MacosOpenGlRenderingAPI::Draw()
    {
        std::cout << "MacosOpenGlRenderingAPI::Draw()" << std::endl;
    }

    void MacosOpenGlRenderingAPI::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void MacosOpenGlRenderingAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}
