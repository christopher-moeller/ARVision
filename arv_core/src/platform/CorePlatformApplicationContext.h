#pragma once

#include "platform/PlatformApplicationContext.h"

namespace arv {
    class CorePlatformApplicationContext : public PlatformApplicationContext
    {
    public:
        CorePlatformApplicationContext() = default;
        ~CorePlatformApplicationContext() = default;

        CorePlatformApplicationContext(Logger* m_logger, EventManager* m_eventManager);

        Logger* GetLogger() override;
        EventManager* GetEventManager() override;

    private:
        Logger* m_logger;
        EventManager* m_eventManager;
    };
}