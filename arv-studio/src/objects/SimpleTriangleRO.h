#pragma once

#include "rendering/RenderingObject.h"
#include "rendering/CoreShaderSource.h"
#include <glm/glm.hpp>
#include <memory>

namespace arv {

    class SimpleTriangleRO : public RenderingObject {

    public:
        SimpleTriangleRO();

        std::shared_ptr<Shader>& GetShader() override;
        std::shared_ptr<VertexArray>& GetVertexArray() override;

        void SetColor(const glm::vec4& color);
        const glm::vec4& GetColor() const { return m_Color; }

    private:
        std::unique_ptr<CoreShaderSource> m_ShaderSource;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
        glm::vec4 m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    };

}
