#include "CameraController.h"


namespace arv {

    CameraController::CameraController(Camera* camera, DeviceType deviceType) : m_Camera(camera), m_DeviceType(deviceType) {
        
    }

    bool CameraControllerAppContext::IsKeyPressed(int keyCode) {
        return m_EventManager->IsKeyPressed(keyCode);
    }

    void CameraController::Init() {
        if(m_DeviceType == DeviceType::DESKTOP_COMPUTER) {
            InitForDesktopComputer();
        } else if(m_DeviceType == DeviceType::MOBILE_DEVICE) {
            InitForMobileDevice();
        }
    }

    void CameraController::UpdateOnStep(CameraControllerAppContext &context) {
        if(m_DeviceType == DeviceType::DESKTOP_COMPUTER) {
            UpdateOnStepForDesktopComputer(context);
        } else if(m_DeviceType == DeviceType::MOBILE_DEVICE) {
            UpdateOnStepForMobileDevice(context);
        }
    }

}