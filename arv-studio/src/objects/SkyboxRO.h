#pragma once

#include "rendering/RenderingObject.h"
#include "rendering/CoreShaderSource.h"
#include <memory>

namespace arv {

    class SkyboxRO : public RenderingObject {
    public:
        SkyboxRO();

        std::shared_ptr<Shader>& GetShader() override;
        std::shared_ptr<VertexArray>& GetVertexArray() override;

    private:
        std::unique_ptr<CoreShaderSource> m_ShaderSource;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
    };

}
