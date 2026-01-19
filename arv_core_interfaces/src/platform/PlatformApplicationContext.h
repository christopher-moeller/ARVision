#pragma once

#include "utils/Logger.h"
#include "events/EventManager.h"

namespace arv {

    class PlattformApplicationContext
    {
    public:
        PlattformApplicationContext() = default;
        ~PlattformApplicationContext() = default;
        
        virtual Logger* GetLogger() = 0;
        virtual EventManager* GetEventManager() = 0;
    };

}
