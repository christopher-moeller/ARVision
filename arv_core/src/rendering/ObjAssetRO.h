#pragma once

#include "RenderingObject.h"
#include "CoreShaderSource.h"
#include "rendering/Texture.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace arv {

    class ObjAssetRO : public RenderingObject {

    public:
        // pathFragment is the folder name inside assets/objects/, e.g. "SMG"
        ObjAssetRO(const std::string& pathFragment);

        std::shared_ptr<Shader>& GetShader() override;
        std::shared_ptr<VertexArray>& GetVertexArray() override;
        std::shared_ptr<Texture2D> GetTexture() override { return m_Texture; }

    private:
        std::unique_ptr<CoreShaderSource> m_ShaderSource;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<Texture2D> m_Texture;

        std::string m_AssetPath;
    };

}
