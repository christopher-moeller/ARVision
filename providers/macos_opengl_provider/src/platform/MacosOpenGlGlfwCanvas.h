#pragma once
#include "platform/Canvas.h"

struct GLFWwindow;

namespace arv
{
    class MacosOpenGlGlfwCanvas : public Canvas
    {
    public:
        MacosOpenGlGlfwCanvas();
        ~MacosOpenGlGlfwCanvas() override;

        void Init(PlatformApplicationContext* context) override;
        void PollEvents() override;
        void SwapBuffers() override;
        void Destroy() override;
        bool ShouldClose() override;

    private:
        GLFWwindow* m_window = nullptr;
    };
}
