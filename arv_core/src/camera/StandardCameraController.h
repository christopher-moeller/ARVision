#pragma once

#include "CameraController.h"
#include "StandardCamera.h"

namespace arv {

    // Type alias for convenience
    using StandardCameraController = CameraController<StandardCamera>;

    // Desktop input strategy for StandardCamera
    class DesktopInputStrategy : public InputStrategy<StandardCamera> {
    public:
        DesktopInputStrategy(float translationSpeed = 5.0f, float rotationSpeed = 45.0f)
            : m_TranslationSpeed(translationSpeed)
            , m_RotationSpeed(rotationSpeed)
        {}

        void Init(StandardCameraController& controller) override;
        void Update(StandardCameraController& controller, CameraControllerAppContext& context) override;

    private:
        float m_TranslationSpeed;
        float m_RotationSpeed;

        void InitResizeHandling(StandardCameraController& controller);
    };

    // Mobile input strategy for StandardCamera
    class MobileInputStrategy : public InputStrategy<StandardCamera> {
    public:
        void Init(StandardCameraController& controller) override;
        void Update(StandardCameraController& controller, CameraControllerAppContext& context) override;

    private:
        void InitResizeHandling(StandardCameraController& controller);
    };

    // Factory function to create controller with appropriate strategy
    std::unique_ptr<StandardCameraController> CreateStandardCameraController(
        StandardCamera* camera,
        bool isDesktop);

}