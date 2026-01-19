#pragma once

#include "utils/Logger.h"
#include "events/EventManager.h"

namespace arv {

    class PlatformApplicationContext
    {
    public:
        PlatformApplicationContext() = default;
        ~PlatformApplicationContext() = default;
        
        virtual Logger* GetLogger() = 0;
        virtual EventManager* GetEventManager() = 0;
    };

}
