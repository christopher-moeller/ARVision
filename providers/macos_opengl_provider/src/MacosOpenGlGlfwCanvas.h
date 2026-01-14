#pragma once
#include "plattform/Canvas.h"

namespace arv
{
    class MacosOpenGlGlfwCanvas : public Canvas
    {
    public:
        MacosOpenGlGlfwCanvas();
        ~MacosOpenGlGlfwCanvas() override;

        void Init() override;
        void PollEvents() override;
        void SwapBuffers() override;
        void Destroy() override;
    };
}
