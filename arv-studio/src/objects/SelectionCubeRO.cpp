#include "SelectionCubeRO.h"
#include "rendering/Renderer.h"

namespace arv {

    static const char* kShaderSource = R"(

        ### GLSL_VERTEX_SHADER ###

        #version 330 core

        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_mvp;

        void main()
        {
            gl_Position = u_mvp * vec4(a_Position, 1.0);
        }

        ### GLSL_FRAGMENT_SHADER ###

        #version 330 core

        layout(location = 0) out vec4 color;

        void main()
        {
            color = vec4(0.0, 0.7, 1.0, 0.3);
        }

        ### MSL_SHADER ###

        #include <metal_stdlib>
        using namespace metal;

        struct VertexUniforms {
            float4x4 u_mvp;
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

        fragment float4 fragmentMain(VertexOut in [[stage_in]]) {
            return float4(0.0, 0.7, 1.0, 0.3);
        }
    )";

    SelectionCubeRO::SelectionCubeRO(Renderer* renderer) {
        // Unit cube vertices: 8 corners from (-0.5, -0.5, -0.5) to (0.5, 0.5, 0.5)
        float vertices[] = {
            // Front face
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            // Back face
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
        };
        // 12 triangles (6 faces x 2 triangles each) = 36 indices
        uint32_t indices[] = {
            // Front
            0, 1, 2,  2, 3, 0,
            // Back
            5, 4, 7,  7, 6, 5,
            // Left
            4, 0, 3,  3, 7, 4,
            // Right
            1, 5, 6,  6, 2, 1,
            // Top
            3, 2, 6,  6, 7, 3,
            // Bottom
            4, 5, 1,  1, 0, 4,
        };
        BufferLayout layout = { { ShaderDataType::Float3, "a_Position" } };

        auto resources = SetupRendering(renderer, kShaderSource,
            vertices, sizeof(vertices), indices, 36, layout);

        m_ShaderSource = std::move(resources.shaderSource);
        m_Shader = std::move(resources.shader);
        m_VertexArray = std::move(resources.vertexArray);

        SetName("SelectionCube");
    }

    std::shared_ptr<Shader>& SelectionCubeRO::GetShader() {
        return m_Shader;
    }

    std::shared_ptr<VertexArray>& SelectionCubeRO::GetVertexArray() {
        return m_VertexArray;
    }

}
