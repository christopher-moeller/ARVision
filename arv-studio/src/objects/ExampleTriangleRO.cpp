#include "ExampleTriangleRO.h"
#include "ARVApplication.h"
#include "rendering/ShaderSource.h"
#include "rendering/CoreShaderSource.h"
#include <string>

namespace arv {

    ExampleTriangleRO::ExampleTriangleRO() {

        ARVApplication* app = ARVApplication::Get();

        std::string fullSource = R"(

            ### GLSL_VERTEX_SHADER ###

            #version 330 core
                            
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            uniform mat4 u_mvp;

            out vec3 v_Position;
            out vec4 v_Color;

            void main()
            {
                v_Position = a_Position;
                v_Color = a_Color;
                gl_Position = u_mvp * vec4(a_Position, 1.0);   
            }

            ### GLSL_FRAGMENT_SHADER ###

            #version 330 core
                            
            layout(location = 0) out vec4 color;

            in vec3 v_Position;
            in vec4 v_Color;

            void main()
            {
                color = vec4(v_Position * 0.5 + 0.5, 1.0);
                color = v_Color;
            }

            ### MSL_SHADER ###

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

        m_ShaderSource = std::make_unique<CoreShaderSource>(fullSource);
        m_Shader.reset(app->GetRenderer()->CreateShader(m_ShaderSource.get()));
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
