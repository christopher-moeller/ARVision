#pragma once

#include "events/EventManager.h"

namespace arv {

    class CoreEventManager : public EventManager {
        
        void PushEvent(Event& event) override;
        void AddListener(EventType type, const std::function<bool(arv::Event&)>& fun) override;
        bool IsKeyPressed(int keyCode) override;
        void SetKeyPressedPollCallback(const std::function<bool(int&)>& fun) override;

    };

}
