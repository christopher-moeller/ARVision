#include "ExampleTriangleRO.h"
#include "ARVApplication.h"
#include "rendering/ShaderSource.h"
#include <string>

namespace arv {

    ExampleTriangleRO::ExampleTriangleRO() {

        ARVApplication* app = ARVApplication::Get();

        // GLSL shaders for OpenGL
        std::string glslVertexSource = R"(
            #version 330 core

            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec4 aColor;

            out vec4 vertexColor;

            void main()
            {
                gl_Position = vec4(aPos, 1.0);
                vertexColor = aColor;
            }
        )";

        std::string glslFragmentSource = R"(
            #version 330 core

            uniform vec4 u_Color;

            in vec4 vertexColor;
            out vec4 FragColor;

            void main()
            {
                FragColor = vertexColor * u_Color;
            }
        )";

        // MSL shader for Metal
        std::string mslSource = R"(
            #include <metal_stdlib>
            using namespace metal;

            struct Uniforms {
                float4 color;
            };

            struct VertexIn {
                float3 position [[attribute(0)]];
                float4 color [[attribute(1)]];
            };

            struct VertexOut {
                float4 position [[position]];
                float4 color;
            };

            vertex VertexOut vertexMain(VertexIn in [[stage_in]]) {
                VertexOut out;
                out.position = float4(in.position, 1.0);
                out.color = in.color;
                return out;
            }

            fragment float4 fragmentMain(VertexOut in [[stage_in]],
                                         constant Uniforms& uniforms [[buffer(0)]]) {
                return in.color * uniforms.color;
            }
        )";

        ShaderSource shaderSource(glslVertexSource, glslFragmentSource, mslSource);
        m_Shader.reset(app->GetRenderer()->CreateShader(shaderSource));
        m_Shader->Compile();
        
        m_VertexArray.reset(app->GetRenderer()->CreateVertexArray());
        
        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };
        
        
        std::shared_ptr<arv::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(app->GetRenderer()->CreateVertexBuffer(vertices, sizeof(vertices)));
        arv::BufferLayout layout = {
                    { arv::ShaderDataType::Float3, "a_Position" },
                    { arv::ShaderDataType::Float4, "a_Color" }
                };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);
        
        uint32_t indices[3] = { 0, 1, 2 };
        std::shared_ptr<arv::IndexBuffer> indexBuffer;
        indexBuffer.reset(app->GetRenderer()->CreateIndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        
        m_VertexArray->Unbind();

    };

    std::shared_ptr<Shader>& ExampleTriangleRO::GetShader() {
        return m_Shader;
    };
    std::shared_ptr<VertexArray>& ExampleTriangleRO::GetVertexArray() {
        return m_VertexArray;
    };

    void ExampleTriangleRO::SetColor(const glm::vec4& color) {
        m_Color = color;
        m_Shader->UploadUniformFloat4("u_Color", m_Color);
    }

}
