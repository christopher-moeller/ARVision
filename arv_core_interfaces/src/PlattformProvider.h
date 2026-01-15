#pragma once

#include "utils/Logger.h"

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
        
        virtual Logger* CreateCustomLogger() {return nullptr;};

    protected:
        Canvas* m_canvas;
        RenderingAPI* m_renderingAPI;
        std::unique_ptr<Logger> m_Logger;
    };
}
