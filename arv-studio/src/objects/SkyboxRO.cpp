#include "SkyboxRO.h"
#include "ARVApplication.h"
#include "rendering/CoreShaderSource.h"
#include <string>

namespace arv {

    SkyboxRO::SkyboxRO() {
        ARVApplication* app = ARVApplication::Get();

        std::string fullSource = R"(

            ### GLSL_VERTEX_SHADER ###

            #version 330 core

            layout(location = 0) in vec3 a_Position;

            out vec2 v_Position;

            void main()
            {
                v_Position = a_Position.xy;
                gl_Position = vec4(a_Position.xy, 0.9999, 1.0);
            }

            ### GLSL_FRAGMENT_SHADER ###

            #version 330 core

            layout(location = 0) out vec4 fragColor;

            in vec2 v_Position;

            uniform mat4 u_inverseVP;
            uniform sampler2D u_Texture;

            const float PI = 3.14159265358979323846;

            void main()
            {
                vec4 clipPos = vec4(v_Position, 1.0, 1.0);
                vec4 worldPos = u_inverseVP * clipPos;
                vec3 dir = normalize(worldPos.xyz / worldPos.w);

                float u = atan(dir.z, dir.x) / (2.0 * PI) + 0.5;
                float v = asin(clamp(dir.y, -1.0, 1.0)) / PI + 0.5;
                v = 1.0 - v;

                vec3 hdrColor = texture(u_Texture, vec2(u, v)).rgb;

                // Reinhard tonemapping
                vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

                // Gamma correction
                mapped = pow(mapped, vec3(1.0 / 2.2));

                fragColor = vec4(mapped, 1.0);
            }

            ### MSL_SHADER ###

            #include <metal_stdlib>
            using namespace metal;

            struct VertexUniforms {
                float4x4 u_inverseVP;
            };

            struct FragmentUniforms {
                float4x4 u_inverseVP;
            };

            struct VertexIn {
                float3 position [[attribute(0)]];
            };

            struct VertexOut {
                float4 position [[position]];
                float2 ndc;
            };

            vertex VertexOut vertexMain(VertexIn in [[stage_in]],
                                        constant VertexUniforms& uniforms [[buffer(1)]]) {
                VertexOut out;
                out.position = float4(in.position.xy, 0.9999, 1.0);
                out.ndc = in.position.xy;
                return out;
            }

            fragment float4 fragmentMain(VertexOut in [[stage_in]],
                                         constant FragmentUniforms& uniforms [[buffer(0)]],
                                         texture2d<float> tex [[texture(0)]],
                                         sampler texSampler [[sampler(0)]]) {
                float4 clipPos = float4(in.ndc, 1.0, 1.0);
                float4 worldPos = uniforms.u_inverseVP * clipPos;
                float3 dir = normalize(worldPos.xyz / worldPos.w);

                constexpr float PI = 3.14159265358979323846;
                float u = atan2(dir.z, dir.x) / (2.0 * PI) + 0.5;
                float v = asin(clamp(dir.y, -1.0f, 1.0f)) / PI + 0.5;
                v = 1.0 - v;

                float3 hdrColor = tex.sample(texSampler, float2(u, v)).rgb;

                // Reinhard tonemapping
                float3 mapped = hdrColor / (hdrColor + float3(1.0));

                // Gamma correction
                mapped = pow(mapped, float3(1.0 / 2.2));

                return float4(mapped, 1.0);
            }
        )";

        m_ShaderSource = std::make_unique<CoreShaderSource>(fullSource);
        m_Shader = app->GetRenderer()->CreateShader(m_ShaderSource.get());
        m_Shader->Compile();

        m_VertexArray = app->GetRenderer()->CreateVertexArray();

        // Fullscreen quad in NDC
        float vertices[] = {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f
        };

        auto vertexBuffer = app->GetRenderer()->CreateVertexBuffer(vertices, sizeof(vertices));
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" }
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
        auto indexBuffer = app->GetRenderer()->CreateIndexBuffer(indices, sizeof(indices) / sizeof(uint32_t));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_VertexArray->Unbind();
    }

    std::shared_ptr<Shader>& SkyboxRO::GetShader() {
        return m_Shader;
    }

    std::shared_ptr<VertexArray>& SkyboxRO::GetVertexArray() {
        return m_VertexArray;
    }

}
