#pragma once

#include "rendering/RenderingObject.h"
#include "rendering/CoreShaderSource.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace arv {

    class Renderer;

    class SimpleTriangleRO : public RenderingObject {

    public:
        SimpleTriangleRO(Renderer* renderer);

        std::shared_ptr<Shader>& GetShader() override;
        std::shared_ptr<VertexArray>& GetVertexArray() override;

        const std::vector<glm::vec3>& GetMeshVertices() const override { return m_MeshVertices; }
        const std::vector<uint32_t>& GetMeshIndices() const override { return m_MeshIndices; }

        void SetColor(const glm::vec4& color);
        const glm::vec4& GetColor() const { return m_Color; }

        void RenderCustomImGui() override;
        void SaveCustomProperties(nlohmann::json& j) const override;

    private:
        std::unique_ptr<CoreShaderSource> m_ShaderSource;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
        glm::vec4 m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        std::vector<glm::vec3> m_MeshVertices;
        std::vector<uint32_t> m_MeshIndices;
    };

}
