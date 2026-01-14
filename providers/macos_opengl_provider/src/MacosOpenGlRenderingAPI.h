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

        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;
    };
}
