#include "StandardCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace arv {

    StandardCamera::StandardCamera(int width, int height) : m_FOV(45.0f), m_AspectRatio(static_cast<float>(width) / static_cast<float>(height)), m_NearPlane(0.1f), m_FarPlane(100.0f),
        m_Position(glm::vec3(0.0f, 0.0f, 3.0f)), m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        m_Up(glm::vec3(0.0f, 1.0f, 0.0f)), m_Yaw(-90.0f), m_Pitch(0.0f) {
        UpdateCameraVectors();
    }

    StandardCamera::StandardCamera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane),
          m_Position(glm::vec3(0.0f, 0.0f, 3.0f)), m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          m_Up(glm::vec3(0.0f, 1.0f, 0.0f)), m_Yaw(-90.0f), m_Pitch(0.0f) {
        UpdateCameraVectors();
    }

    glm::mat4 StandardCamera::GetViewMatrix() {
        return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    glm::mat4 StandardCamera::GetProjectionMatrix() {
        return glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
    }

    void StandardCamera::SetPosition(const glm::vec3& position) {
        m_Position = position;
    }

    void StandardCamera::SetRotation(float yaw, float pitch) {
        m_Yaw = yaw;
        m_Pitch = pitch;
        UpdateCameraVectors();
    }

    void StandardCamera::SetAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
    }

    /*
    void StandardCamera::Rotate(float yawOffset, float pitchOffset) {
        m_Yaw += yawOffset;
        m_Pitch += pitchOffset;
        m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);
        UpdateCameraVectors();
    }
     */

    void StandardCamera::UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);

        m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

}


