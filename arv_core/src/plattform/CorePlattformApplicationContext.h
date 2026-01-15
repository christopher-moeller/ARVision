#pragma once

#include "plattform/PlattformApplicationContext.h"

namespace arv {
    class CorePlattformApplicationContext : public PlattformApplicationContext
    {
    public:
        CorePlattformApplicationContext() = default;
        ~CorePlattformApplicationContext() = default;

        CorePlattformApplicationContext(Logger* m_logger, EventManager* m_eventManager);

        Logger* GetLogger() override;
        EventManager* GetEventManager() override;

    private:
        Logger* m_logger;
        EventManager* m_eventManager;
    };
}