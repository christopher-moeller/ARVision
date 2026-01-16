#pragma once

#include "CameraController.h"
#include "StandardCamera.h"

namespace arv {

    class StandardCameraController : public CameraController {
    public:
        StandardCameraController(StandardCamera* standardCamera, DeviceType deviceType) : CameraController(standardCamera, deviceType), m_StandardCamera(standardCamera) {}
        
    protected:
        void InitForMobileDevice() override;
        void InitForDesktopComputer() override;
        void UpdateOnStepForDesktopComputer(CameraControllerAppContext &context) override;
        
    private:
        StandardCamera* m_StandardCamera;
        float m_CameraTranslationSpeed = 5.0f;
        float m_CameraRotationSpeed = 45.0f;
        
        void InitResizeHandling();
        
    };

}