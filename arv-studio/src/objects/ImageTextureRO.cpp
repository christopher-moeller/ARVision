#include "ImageTextureRO.h"
#include "ARVApplication.h"
#include "rendering/ShaderSource.h"
#include "rendering/CoreShaderSource.h"
#include <string>

namespace arv {

    ImageTextureRO::ImageTextureRO(const std::string& texturePath) {

        ARVApplication* app = ARVApplication::Get();

        std::string fullSource = R"(

            ### GLSL_VERTEX_SHADER ###

            #version 330 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec2 a_TexCoord;

            uniform mat4 u_mvp;

            out vec2 v_TexCoord;

            void main()
            {
                v_TexCoord = a_TexCoord;
                gl_Position = u_mvp * vec4(a_Position, 1.0);
            }

            ### GLSL_FRAGMENT_SHADER ###

            #version 330 core

            layout(location = 0) out vec4 color;

            in vec2 v_TexCoord;

            uniform sampler2D u_Texture;

            void main()
            {
                color = texture(u_Texture, v_TexCoord);
            }

            ### MSL_SHADER ###

            #include <metal_stdlib>
            using namespace metal;

            struct VertexUniforms {
                float4x4 u_mvp;
            };

            struct VertexIn {
                float3 position [[attribute(0)]];
                float2 texCoord [[attribute(1)]];
            };

            struct VertexOut {
                float4 position [[position]];
                float2 texCoord;
            };

            vertex VertexOut vertexMain(VertexIn in [[stage_in]],
                                        constant VertexUniforms& uniforms [[buffer(1)]]) {
                VertexOut out;
                out.position = uniforms.u_mvp * float4(in.position, 1.0);
                // Flip V coordinate (Metal texture origin is top-left, OpenGL is bottom-left)
                out.texCoord = float2(in.texCoord.x, 1.0 - in.texCoord.y);
                return out;
            }

            fragment float4 fragmentMain(VertexOut in [[stage_in]],
                                         texture2d<float> tex [[texture(0)]],
                                         sampler texSampler [[sampler(0)]]) {
                return tex.sample(texSampler, in.texCoord);
            }
        )";

        m_ShaderSource = std::make_unique<CoreShaderSource>(fullSource);
        m_Shader = app->GetRenderer()->CreateShader(m_ShaderSource.get());
        m_Shader->Compile();

        m_VertexArray = app->GetRenderer()->CreateVertexArray();

        // Quad vertices: position (xyz) + texture coordinates (uv)
        float vertices[] = {
            // Position            // TexCoord
            -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,  // Bottom-left
             0.5f, -0.5f, 0.0f,    1.0f, 0.0f,  // Bottom-right
             0.5f,  0.5f, 0.0f,    1.0f, 1.0f,  // Top-right
            -0.5f,  0.5f, 0.0f,    0.0f, 1.0f   // Top-left
        };

        auto vertexBuffer = app->GetRenderer()->CreateVertexBuffer(vertices, sizeof(vertices));
        arv::BufferLayout layout = {
            { arv::ShaderDataType::Float3, "a_Position" },
            { arv::ShaderDataType::Float2, "a_TexCoord" }
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
        auto indexBuffer = app->GetRenderer()->CreateIndexBuffer(indices, sizeof(indices) / sizeof(uint32_t));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_VertexArray->Unbind();

        // Load the texture
        m_Texture = app->GetRenderer()->CreateTexture2D(texturePath);
    };

    std::shared_ptr<Shader>& ImageTextureRO::GetShader() {
        return m_Shader;
    };
    std::shared_ptr<VertexArray>& ImageTextureRO::GetVertexArray() {
        return m_VertexArray;
    };

}
