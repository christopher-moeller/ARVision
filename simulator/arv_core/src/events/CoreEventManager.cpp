#include "ARVBase.h"
#include "CoreEventManager.h"


namespace arv {

    void CoreEventManager::PushEvent(Event& event) {
        ARV_LOG_INFO("CoreEventManager::PushEvent() - Event received: {}", event);

        if(m_Listeners.find(event.GetType()) == m_Listeners.end()) {
            ARV_LOG_INFO("CoreEventManager::PushEvent() - No listeners registered for this event type");
            return;
        }

        std::vector<std::function<bool(Event&)>> listeners = m_Listeners[event.GetType()];
        ARV_LOG_INFO("CoreEventManager::PushEvent() - Dispatching to {} listener(s)", listeners.size());
        for (int i = 0; i < listeners.size(); i++) {
            std::function<bool(Event&)> currentFun = listeners[i];
            bool result = currentFun(*(&event));
            if (result) {
                ARV_LOG_INFO("CoreEventManager::PushEvent() - Event handled by listener {}", i);
            }
        }
    }

    void CoreEventManager::AddListener(EventType type, const std::function<bool(arv::Event&)>& fun) {
        ARV_LOG_INFO("CoreEventManager::AddListener() - Adding listener for event type {}", static_cast<int>(type));
        if(m_Listeners.find(type) == m_Listeners.end()) {
            std::vector<std::function<bool(Event&)>> emptyVector;
            m_Listeners.insert({type, emptyVector});
            ARV_LOG_INFO("CoreEventManager::AddListener() - Created new listener list for event type");
        }

        m_Listeners.at(type).push_back(fun);
        ARV_LOG_INFO("CoreEventManager::AddListener() - Now have {} listener(s) for this event type", m_Listeners.at(type).size());
    }

    bool CoreEventManager::IsKeyPressed(int keyCode) {
        if(m_KeyPressedPollCallback) {
            return m_KeyPressedPollCallback(keyCode);
        }
        return false;
    }

    void CoreEventManager::SetKeyPressedPollCallback(const std::function<bool(int&)>& fun) {
        ARV_LOG_INFO("CoreEventManager::SetKeyPressedPollCallback() - Key pressed poll callback set");
        m_KeyPressedPollCallback = fun;
    }

}