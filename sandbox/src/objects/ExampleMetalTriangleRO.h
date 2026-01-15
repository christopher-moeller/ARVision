#pragma once

#include "rendering/RenderingObject.h"

namespace arv {

    class ExampleMetalTriangleRO : public RenderingObject {

    public:
        ExampleMetalTriangleRO();

        std::shared_ptr<Shader>& GetShader() override;
        std::shared_ptr<VertexArray>& GetVertexArray() override;

    private:
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;

    };

}
