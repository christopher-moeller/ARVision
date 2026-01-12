#pragma once
#include "plattform/PlattformCanvas.h"

namespace arv
{
    class TestCanvas : public arv::PlattformCanvas
    {
    public:
        virtual ~TestCanvas() = default;

        virtual void InitializeCanvas() override;
        virtual void PollEvents() override;
        virtual void SwapBuffers() override;
        virtual void DestroyCanvas() override;
    };
}