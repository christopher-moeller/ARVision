#pragma once
#include "rendering/RenderingAPI.h"

namespace arv
{
    class MacosOpenGlRenderingAPI : public RenderingAPI
    {
    public:
        MacosOpenGlRenderingAPI();
        ~MacosOpenGlRenderingAPI() override;

        void Init() override;
        void Draw() override;
    };
}
