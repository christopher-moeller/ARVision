#pragma once

#include "platform/PlatformApplicationContext.h"

namespace arv {
    class CorePlatformApplicationContext : public PlatformApplicationContext
    {
    public:
        CorePlatformApplicationContext() = default;
        ~CorePlatformApplicationContext() = default;

        CorePlatformApplicationContext(Logger* logger, EventManager* eventManager,
                                       int windowWidth = 1280, int windowHeight = 720);

        Logger* GetLogger() override;
        EventManager* GetEventManager() override;
        int GetWindowWidth() const override { return m_windowWidth; }
        int GetWindowHeight() const override { return m_windowHeight; }

    private:
        Logger* m_logger;
        EventManager* m_eventManager;
        int m_windowWidth = 1280;
        int m_windowHeight = 720;
    };
}