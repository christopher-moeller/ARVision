#pragma once

#include "rendering/RenderingObject.h"
#include "rendering/CoreShaderSource.h"
#include "rendering/Texture.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace arv {

    class Renderer;

    class ImageTextureRO : public RenderingObject {

    public:
        ImageTextureRO(Renderer* renderer, const std::string& texturePath);

        std::shared_ptr<Shader>& GetShader() override;
        std::shared_ptr<VertexArray>& GetVertexArray() override;
        std::shared_ptr<Texture2D> GetTexture() override { return m_Texture; }

        const std::vector<glm::vec3>& GetMeshVertices() const override { return m_MeshVertices; }
        const std::vector<uint32_t>& GetMeshIndices() const override { return m_MeshIndices; }

    private:
        std::unique_ptr<CoreShaderSource> m_ShaderSource;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<Texture2D> m_Texture;

        std::vector<glm::vec3> m_MeshVertices;
        std::vector<uint32_t> m_MeshIndices;
    };

}
