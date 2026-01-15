#include "CorePlattformApplicationContext.h"


namespace arv {

    CorePlattformApplicationContext::CorePlattformApplicationContext(Logger* logger, EventManager* eventManager)
        : m_logger(logger), m_eventManager(eventManager)
    {

    }

    Logger* CorePlattformApplicationContext::GetLogger()
    {
        return m_logger;
    }

    EventManager* CorePlattformApplicationContext::GetEventManager()
    {
        return m_eventManager;
    }

}