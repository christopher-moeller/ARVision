#pragma once

namespace arv
{
    class RenderingAPI
    {
    public:
        RenderingAPI() = default;
        virtual ~RenderingAPI() = default;

        virtual void Init() = 0;
        virtual void Draw() = 0;
    };
}
