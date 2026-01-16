#pragma once

#include <glm/glm.hpp>

namespace arv {

    class Camera {
    public:
        virtual glm::mat4 GetViewMatrix() = 0;
        virtual glm::mat4 GetProjectionMatrix() = 0;
        virtual glm::mat4 GetViewProjectionMatrix() { return this->GetProjectionMatrix() * this->GetViewMatrix(); }
        
    };

}