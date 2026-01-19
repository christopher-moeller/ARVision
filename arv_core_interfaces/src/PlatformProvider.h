#pragma once

#include <memory>
#include "utils/Logger.h"
#include "platform/PlatformApplicationContext.h"
#include "platform/Canvas.h"
#include "rendering/RenderingAPI.h"

namespace arv
{

    class PlatformProvider
    {
    public:
        PlatformProvider() = default;
        virtual ~PlatformProvider() = default;

        virtual void Init(PlatformApplicationContext* context) = 0;

        Canvas* GetCanvas() const { return m_canvas.get(); }
        RenderingAPI* GetRenderingAPI() const { return m_renderingAPI.get(); }

        virtual Logger* CreateCustomLogger() {return nullptr;};

    protected:
        std::unique_ptr<Canvas> m_canvas;
        std::unique_ptr<RenderingAPI> m_renderingAPI;
        std::unique_ptr<Logger> m_Logger;
    };
}
