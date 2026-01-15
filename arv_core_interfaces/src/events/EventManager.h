#pragma once

#include "Event.h"
#include <map>
#include <vector>
#include "utils/KeyCodes.h"

namespace arv {

    class EventManager {
        
    public:
        virtual ~EventManager() = default;

        virtual void PushEvent(Event& event) = 0;
        virtual void AddListener(EventType type, const std::function<bool(arv::Event&)>& fun) = 0;
        virtual bool IsKeyPressed(int keyCode) = 0;
        virtual void SetKeyPressedPollCallback(const std::function<bool(int&)>& fun) = 0;
        
    protected:
        std::map<EventType, std::vector<std::function<bool(Event&)>>> m_Listeners;
        std::function<bool(int&)> m_KeyPressedPollCallback;
    };

}