#pragma once

#include <glm/glm.hpp>
#include "Camera.h"

namespace arv {

    class StandardCamera : public Camera {
    public:
        StandardCamera(int width, int height);
        StandardCamera(float fov, float aspectRatio, float nearPlane, float farPlane);

        glm::mat4 GetViewMatrix() const override;
        glm::mat4 GetProjectionMatrix() const override;

        void SetPosition(const glm::vec3& position);
        void SetRotation(float yaw, float pitch);
        void SetAspectRatio(float aspectRatio);
        void SetWorldUp(const glm::vec3& worldUp);

        glm::vec3 GetPosition() const { return m_Position; }
        glm::vec3 GetFront() const { return m_Front; }
        glm::vec3 GetRight() const { return m_Right; }
        glm::vec3 GetUp() const { return m_Up; }
        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }

    private:
        void UpdateCameraVectors();

        glm::vec3 m_Position;
        glm::vec3 m_Front;
        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_WorldUp;

        float m_Yaw;
        float m_Pitch;

        float m_FOV;
        float m_AspectRatio;
        float m_NearPlane;
        float m_FarPlane;

        static constexpr float PITCH_MIN = -89.0f;
        static constexpr float PITCH_MAX = 89.0f;
    };

}