#include "StandardCameraController.h"
#include "utils/KeyCodes.h"
#include <glm/glm.hpp>
#include "../ARVBase.h"

namespace arv {

    // DesktopInputStrategy implementation

    void DesktopInputStrategy::Init(StandardCameraController& controller) {
        InitResizeHandling(controller);
    }

    void DesktopInputStrategy::Update(StandardCameraController& controller, CameraControllerAppContext& context) {
        StandardCamera* camera = controller.GetCamera();

        glm::vec3 frontDirection = camera->GetFront();
        glm::vec3 rightDirection = camera->GetRight();
        glm::vec3 newPosition = camera->GetPosition();

        float translationVelocity = m_TranslationSpeed * context.GetTimestep();

        if (context.IsKeyPressed(ARV_KEY_UP)) {
            newPosition += frontDirection * translationVelocity;
        }
        if (context.IsKeyPressed(ARV_KEY_DOWN)) {
            newPosition -= frontDirection * translationVelocity;
        }
        if (context.IsKeyPressed(ARV_KEY_LEFT)) {
            newPosition -= rightDirection * translationVelocity;
        }
        if (context.IsKeyPressed(ARV_KEY_RIGHT)) {
            newPosition += rightDirection * translationVelocity;
        }

        camera->SetPosition(newPosition);

        float rotationVelocity = m_RotationSpeed * context.GetTimestep();
        float pitch = camera->GetPitch();
        float yaw = camera->GetYaw();

        if (context.IsKeyPressed(ARV_KEY_W)) {
            pitch += rotationVelocity;
        }
        if (context.IsKeyPressed(ARV_KEY_S)) {
            pitch -= rotationVelocity;
        }
        if (context.IsKeyPressed(ARV_KEY_D)) {
            yaw += rotationVelocity;
        }
        if (context.IsKeyPressed(ARV_KEY_A)) {
            yaw -= rotationVelocity;
        }

        camera->SetRotation(yaw, pitch);
    }

    void DesktopInputStrategy::InitResizeHandling(StandardCameraController& controller) {
        StandardCamera* camera = controller.GetCamera();
        ARVApplication::Get()->GetEventManager()->AddListener(
            EventType::ApplicationResizeEvent,
            [camera](Event& event) {
                auto* resizeEvent = static_cast<ApplicationResizeEvent*>(&event);
                float aspectRatio = static_cast<float>(resizeEvent->GetWidth()) /
                                    static_cast<float>(resizeEvent->GetHeight());
                camera->SetAspectRatio(aspectRatio);
                return false;
            });
    }

    // MobileInputStrategy implementation

    void MobileInputStrategy::Init(StandardCameraController& controller) {
        InitResizeHandling(controller);
    }

    void MobileInputStrategy::Update(StandardCameraController& controller, CameraControllerAppContext& context) {
        // Mobile input handling - touch/gyro controls can be added here
    }

    void MobileInputStrategy::InitResizeHandling(StandardCameraController& controller) {
        StandardCamera* camera = controller.GetCamera();
        ARVApplication::Get()->GetEventManager()->AddListener(
            EventType::ApplicationResizeEvent,
            [camera](Event& event) {
                auto* resizeEvent = static_cast<ApplicationResizeEvent*>(&event);
                float aspectRatio = static_cast<float>(resizeEvent->GetWidth()) /
                                    static_cast<float>(resizeEvent->GetHeight());
                camera->SetAspectRatio(aspectRatio);
                return false;
            });
    }

    // Factory function

    std::unique_ptr<StandardCameraController> CreateStandardCameraController(
        StandardCamera* camera,
        bool isDesktop)
    {
        std::unique_ptr<InputStrategy<StandardCamera>> strategy;
        if (isDesktop) {
            strategy = std::make_unique<DesktopInputStrategy>();
        } else {
            strategy = std::make_unique<MobileInputStrategy>();
        }
        return std::make_unique<StandardCameraController>(camera, std::move(strategy));
    }

}
