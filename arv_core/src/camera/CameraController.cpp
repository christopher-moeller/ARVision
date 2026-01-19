#include "CameraController.h"

namespace arv {

    bool CameraControllerAppContext::IsKeyPressed(int keyCode) {
        return m_EventManager->IsKeyPressed(keyCode);
    }

}