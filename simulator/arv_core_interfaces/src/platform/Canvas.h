#pragma once

#include "platform/PlatformApplicationContext.h"

namespace arv
{
    class Canvas
    {
    public:
        Canvas() = default;
        virtual ~Canvas() = default;

        virtual void Init(PlatformApplicationContext* context) = 0;
        virtual void PollEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual void Destroy() = 0;
        virtual bool ShouldClose() = 0;

        // Returns the native window handle (e.g., GLFWwindow*)
        virtual void* GetNativeWindow() const = 0;
    };
}
