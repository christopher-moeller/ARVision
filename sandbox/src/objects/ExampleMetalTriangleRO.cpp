#include "ExampleMetalTriangleRO.h"
#include "ARVApplication.h"
#include <string>

namespace arv {

    ExampleMetalTriangleRO::ExampleMetalTriangleRO() {

        ARVApplication* app = ARVApplication::Get();

        // Metal Shading Language (MSL) shader
        // Note: Vertex source contains includes and shared structs
        // Fragment source only contains the fragment function (they get combined)
        std::string vertexShaderSource = R"(
            #include <metal_stdlib>
            using namespace metal;

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
        )";

        // Fragment source - no includes or struct redefinitions (already in vertex source)
        std::string fragmentShaderSource = R"(
            fragment float4 fragmentMain(VertexOut in [[stage_in]]) {
                return in.color;
            }
        )";

        m_Shader.reset(app->GetRenderer()->CreateShader(vertexShaderSource, fragmentShaderSource));
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

    std::shared_ptr<Shader>& ExampleMetalTriangleRO::GetShader() {
        return m_Shader;
    };
    std::shared_ptr<VertexArray>& ExampleMetalTriangleRO::GetVertexArray() {
        return m_VertexArray;
    };

}
