#pragma once

#include <glm/glm.hpp>

namespace arv
{
    class RenderingAPI
    {
    public:
        RenderingAPI() = default;
        virtual ~RenderingAPI() = default;

        virtual void Init() = 0;
        virtual void Draw() = 0;
        
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;
    };
}
