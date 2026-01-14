#pragma once

namespace arv
{
    class Canvas;
    class RenderingAPI;

    class PlattformProvider
    {
    public:
        PlattformProvider() = default;
        virtual ~PlattformProvider() = default;

        virtual void Init() = 0;

        Canvas* GetCanvas() const { return m_canvas; }
        RenderingAPI* GetRenderingAPI() const { return m_renderingAPI; }

    protected:
        Canvas* m_canvas = nullptr;
        RenderingAPI* m_renderingAPI = nullptr;
    };
}
