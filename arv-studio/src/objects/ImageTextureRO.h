#pragma once

#include "rendering/RenderingObject.h"
#include "rendering/CoreShaderSource.h"
#include "rendering/Texture.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace arv {

    class ImageTextureRO : public RenderingObject {

    public:
        ImageTextureRO(const std::string& texturePath);

        std::shared_ptr<Shader>& GetShader() override;
        std::shared_ptr<VertexArray>& GetVertexArray() override;
        std::shared_ptr<Texture2D> GetTexture() override { return m_Texture; }

    private:
        std::unique_ptr<CoreShaderSource> m_ShaderSource;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<Texture2D> m_Texture;
    };

}
