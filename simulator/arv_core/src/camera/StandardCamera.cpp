#include "StandardCamera.h"
#include "ARVBase.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace arv {

    StandardCamera::StandardCamera(int width, int height)
        : m_FOV(45.0f)
        , m_AspectRatio(static_cast<float>(width) / static_cast<float>(height))
        , m_NearPlane(0.1f)
        , m_FarPlane(100.0f)
        , m_Position(0.0f, 0.0f, 3.0f)
        , m_Front(0.0f, 0.0f, -1.0f)
        , m_Up(0.0f, 1.0f, 0.0f)
        , m_Right(1.0f, 0.0f, 0.0f)
        , m_WorldUp(0.0f, 1.0f, 0.0f)
        , m_Yaw(-90.0f)
        , m_Pitch(0.0f)
    {
        ARV_LOG_INFO("StandardCamera::StandardCamera() - Creating camera with viewport {}x{}, aspect ratio {:.2f}",
                     width, height, m_AspectRatio);
        UpdateCameraVectors();
    }

    StandardCamera::StandardCamera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_FOV(fov)
        , m_AspectRatio(aspectRatio)
        , m_NearPlane(nearPlane)
        , m_FarPlane(farPlane)
        , m_Position(0.0f, 0.0f, 3.0f)
        , m_Front(0.0f, 0.0f, -1.0f)
        , m_Up(0.0f, 1.0f, 0.0f)
        , m_Right(1.0f, 0.0f, 0.0f)
        , m_WorldUp(0.0f, 1.0f, 0.0f)
        , m_Yaw(-90.0f)
        , m_Pitch(0.0f)
    {
        ARV_LOG_INFO("StandardCamera::StandardCamera() - Creating camera with FOV {:.1f}, aspect ratio {:.2f}, near {:.2f}, far {:.1f}",
                     fov, aspectRatio, nearPlane, farPlane);
        UpdateCameraVectors();
    }

    glm::mat4 StandardCamera::GetViewMatrix() const {
        return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    glm::mat4 StandardCamera::GetProjectionMatrix() const {
        return glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
    }

    void StandardCamera::SetPosition(const glm::vec3& position) {
        m_Position = position;
    }

    void StandardCamera::SetRotation(float yaw, float pitch) {
        m_Yaw = yaw;
        m_Pitch = std::clamp(pitch, PITCH_MIN, PITCH_MAX);
        UpdateCameraVectors();
    }

    void StandardCamera::SetAspectRatio(float aspectRatio) {
        ARV_LOG_INFO("StandardCamera::SetAspectRatio() - Aspect ratio changed to {:.2f}", aspectRatio);
        m_AspectRatio = aspectRatio;
    }

    void StandardCamera::SetWorldUp(const glm::vec3& worldUp) {
        ARV_LOG_INFO("StandardCamera::SetWorldUp() - World up vector changed to ({:.2f}, {:.2f}, {:.2f})",
                     worldUp.x, worldUp.y, worldUp.z);
        m_WorldUp = glm::normalize(worldUp);
        UpdateCameraVectors();
    }

    void StandardCamera::UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);

        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

}
