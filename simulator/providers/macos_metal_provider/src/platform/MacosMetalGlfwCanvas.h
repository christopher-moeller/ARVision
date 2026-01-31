#pragma once
#include "platform/Canvas.h"

struct GLFWwindow;

#ifdef __OBJC__
@class CAMetalLayer;
#else
typedef void CAMetalLayer;
#endif

namespace arv
{
    class MacosMetalGlfwCanvas : public Canvas
    {
    public:
        MacosMetalGlfwCanvas();
        ~MacosMetalGlfwCanvas() override;

        void Init(PlatformApplicationContext* context) override;
        void PollEvents() override;
        void SwapBuffers() override;
        void Destroy() override;
        bool ShouldClose() override;
        void* GetNativeWindow() const override { return m_window; }

        CAMetalLayer* GetMetalLayer() const { return m_metalLayer; }

    private:
        GLFWwindow* m_window = nullptr;
        CAMetalLayer* m_metalLayer = nullptr;
    };
}
