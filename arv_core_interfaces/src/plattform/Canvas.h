#pragma once

namespace arv
{
    class Canvas
    {
    public:
        Canvas() = default;
        virtual ~Canvas() = default;

        virtual void Init() = 0;
        virtual void PollEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual void Destroy() = 0;
    };
}
