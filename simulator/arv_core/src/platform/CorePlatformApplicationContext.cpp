#include "CorePlatformApplicationContext.h"


namespace arv {

    CorePlatformApplicationContext::CorePlatformApplicationContext(Logger* logger, EventManager* eventManager,
                                                                   int windowWidth, int windowHeight)
        : m_logger(logger), m_eventManager(eventManager)
        , m_windowWidth(windowWidth), m_windowHeight(windowHeight)
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