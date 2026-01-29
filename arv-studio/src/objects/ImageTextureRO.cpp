#include "ImageTextureRO.h"
#include "rendering/Renderer.h"

namespace arv {

    static const char* kShaderSource = R"(

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
            if (color.a < 0.01)
                discard;
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
            float4 color = tex.sample(texSampler, in.texCoord);
            if (color.a < 0.01)
                discard_fragment();
            return color;
        }
    )";

    ImageTextureRO::ImageTextureRO(Renderer* renderer, const std::string& texturePath) {
        // Quad vertices: position (xyz) + texture coordinates (uv)
        float vertices[] = {
            // Position            // TexCoord
            -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,  // Bottom-left
             0.5f, -0.5f, 0.0f,    1.0f, 0.0f,  // Bottom-right
             0.5f,  0.5f, 0.0f,    1.0f, 1.0f,  // Top-right
            -0.5f,  0.5f, 0.0f,    0.0f, 1.0f   // Top-left
        };
        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" }
        };

        auto resources = SetupRendering(renderer, kShaderSource,
            vertices, sizeof(vertices), indices, 6, layout);

        m_ShaderSource = std::move(resources.shaderSource);
        m_Shader = std::move(resources.shader);
        m_VertexArray = std::move(resources.vertexArray);

        m_boundsMin = glm::vec3(-0.5f, -0.5f, 0.0f);
        m_boundsMax = glm::vec3(0.5f, 0.5f, 0.0f);

        // Store mesh data for export (quad = 4 vertices, 2 triangles = 6 indices)
        m_MeshVertices = {
            glm::vec3(-0.5f, -0.5f, 0.0f),  // Bottom-left
            glm::vec3( 0.5f, -0.5f, 0.0f),  // Bottom-right
            glm::vec3( 0.5f,  0.5f, 0.0f),  // Top-right
            glm::vec3(-0.5f,  0.5f, 0.0f)   // Top-left
        };
        m_MeshIndices = { 0, 1, 2, 2, 3, 0 };

        m_Texture = renderer->CreateTexture2D(texturePath);
    }

    std::shared_ptr<Shader>& ImageTextureRO::GetShader() {
        return m_Shader;
    };
    std::shared_ptr<VertexArray>& ImageTextureRO::GetVertexArray() {
        return m_VertexArray;
    };

}
