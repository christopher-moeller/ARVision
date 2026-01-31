#pragma once

#include <glm/glm.hpp>

namespace arv {

    class Camera {
    public:
        virtual ~Camera() = default;

        virtual glm::mat4 GetViewMatrix() const = 0;
        virtual glm::mat4 GetProjectionMatrix() const = 0;
        virtual glm::mat4 GetViewProjectionMatrix() const { return GetProjectionMatrix() * GetViewMatrix(); }
    };

}