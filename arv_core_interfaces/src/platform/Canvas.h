#pragma once

#include "plattform/PlattformApplicationContext.h"

namespace arv
{
    class Canvas
    {
    public:
        Canvas() = default;
        virtual ~Canvas() = default;

        virtual void Init(PlattformApplicationContext* context) = 0;
        virtual void PollEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual void Destroy() = 0;
        virtual bool ShouldClose() = 0;
    };
}
