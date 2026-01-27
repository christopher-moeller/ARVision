#include "SimpleTriangleRO.h"
#include "ARVApplication.h"
#include "rendering/ShaderSource.h"
#include "rendering/CoreShaderSource.h"
#include <string>

namespace arv {

    SimpleTriangleRO::SimpleTriangleRO() {

        ARVApplication* app = ARVApplication::Get();

        std::string fullSource = R"(

            ### GLSL_VERTEX_SHADER ###

            #version 330 core
                            
            layout(location = 0) in vec3 a_Position;

            uniform mat4 u_mvp;

            out vec3 v_Position;

            void main()
            {
                v_Position = a_Position;
                gl_Position = u_mvp * vec4(a_Position, 1.0);   
            }

            ### GLSL_FRAGMENT_SHADER ###

            #version 330 core

            layout(location = 0) out vec4 color;

            in vec3 v_Position;

            uniform vec4 u_Color;

            void main()
            {
                color = u_Color;
            }

            ### MSL_SHADER ###

            #include <metal_stdlib>
            using namespace metal;

            struct VertexUniforms {
                float4x4 u_mvp;
            };

            struct FragmentUniforms {
                float4 u_Color;
            };

            struct VertexIn {
                float3 position [[attribute(0)]];
            };

            struct VertexOut {
                float4 position [[position]];
            };

            vertex VertexOut vertexMain(VertexIn in [[stage_in]],
                                        constant VertexUniforms& uniforms [[buffer(1)]]) {
                VertexOut out;
                out.position = uniforms.u_mvp * float4(in.position, 1.0);
                return out;
            }

            fragment float4 fragmentMain(VertexOut in [[stage_in]],
                                         constant FragmentUniforms& uniforms [[buffer(0)]]) {
                return  uniforms.u_Color;
            }
        )";

        m_ShaderSource = std::make_unique<CoreShaderSource>(fullSource);
        m_Shader = app->GetRenderer()->CreateShader(m_ShaderSource.get());
        m_Shader->Compile();

        m_VertexArray = app->GetRenderer()->CreateVertexArray();

        float vertices[3 * 3] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        m_boundsMin = glm::vec3(-0.5f, -0.5f, 0.0f);
        m_boundsMax = glm::vec3(0.5f, 0.5f, 0.0f);

        auto vertexBuffer = app->GetRenderer()->CreateVertexBuffer(vertices, sizeof(vertices));
        arv::BufferLayout layout = {
                    { arv::ShaderDataType::Float3, "a_Position" }
                };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = { 0, 1, 2 };
        auto indexBuffer = app->GetRenderer()->CreateIndexBuffer(indices, sizeof(indices) / sizeof(uint32_t));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        
        m_VertexArray->Unbind();

    };

    std::shared_ptr<Shader>& SimpleTriangleRO::GetShader() {
        return m_Shader;
    };
    std::shared_ptr<VertexArray>& SimpleTriangleRO::GetVertexArray() {
        return m_VertexArray;
    };

    void SimpleTriangleRO::SetColor(const glm::vec4& color) {
        m_Color = color;
        m_Shader->UploadUniformFloat4("u_Color", m_Color);
    }

}
