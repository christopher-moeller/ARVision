#include "CorePlatformApplicationContext.h"


namespace arv {

    CorePlatformApplicationContext::CorePlatformApplicationContext(Logger* logger, EventManager* eventManager)
        : m_logger(logger), m_eventManager(eventManager)
    {

    }

    Logger* CorePlatformApplicationContext::GetLogger()
    {
        return m_logger;
    }

    EventManager* CorePlatformApplicationContext::GetEventManager()
    {
        return m_eventManager;
    }

}