#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "rendering/Shader.h"
#include "rendering/VertexArray.h"
#include "rendering/Texture.h"

namespace arv {

    class RenderingObject {

    public:
        virtual ~RenderingObject() = default;

        virtual std::shared_ptr<Shader>& GetShader() = 0;
        virtual std::shared_ptr<VertexArray>& GetVertexArray() = 0;
        virtual std::shared_ptr<Texture2D> GetTexture() { return nullptr; }

        glm::vec3& GetPosition() { return position; }
        const glm::vec3& GetPosition() const { return position; }
        void SetPosition(const glm::vec3& pos) { position = pos; }
        
        const std::string& GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

    protected:
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        std::string m_name;
    };

}
