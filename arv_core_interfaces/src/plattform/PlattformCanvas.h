#pragma once

namespace arv
{
    class PlattformCanvas
    {
    public:
        virtual ~PlattformCanvas() = default;

        virtual void InitializeCanvas() = 0;
        virtual void PollEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual void DestroyCanvas() = 0;
    };
}