#pragma once

#include <memory>
#include "utils/Logger.h"
#include "plattform/PlattformApplicationContext.h"
#include "plattform/Canvas.h"
#include "rendering/RenderingAPI.h"

namespace arv
{

    class PlattformProvider
    {
    public:
        PlattformProvider() = default;
        virtual ~PlattformProvider() = default;

        virtual void Init(PlattformApplicationContext* context) = 0;

        Canvas* GetCanvas() const { return m_canvas.get(); }
        RenderingAPI* GetRenderingAPI() const { return m_renderingAPI.get(); }

        virtual Logger* CreateCustomLogger() {return nullptr;};

    protected:
        std::unique_ptr<Canvas> m_canvas;
        std::unique_ptr<RenderingAPI> m_renderingAPI;
        std::unique_ptr<Logger> m_Logger;
    };
}
