#include "ObjAssetRO.h"
#include "ARVApplication.h"
#include "rendering/ShaderSource.h"
#include "rendering/CoreShaderSource.h"
#include "utils/AssetPath.h"
#include "ARVBase.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <unordered_map>
#include <algorithm>
#include <cctype>

namespace arv {

    ObjAssetRO::ObjAssetRO(const std::string& pathFragment) {

        ARVApplication* app = ARVApplication::Get();

        // Build paths - use lowercase for the obj filename
        m_AssetPath = AssetPath::Resolve("objects/" + pathFragment);
        std::string lowercaseName = pathFragment;
        std::transform(lowercaseName.begin(), lowercaseName.end(), lowercaseName.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        std::string objPath = m_AssetPath + "/" + lowercaseName + ".obj";

        ARV_LOG_INFO("ObjAssetRO: Loading OBJ from {}", objPath);

        // Load OBJ file using tinyobjloader
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        // Use the asset folder as the material search path
        bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                         objPath.c_str(), (m_AssetPath + "/").c_str());

        if (!warn.empty()) {
            ARV_LOG_INFO("ObjAssetRO warning: {}", warn);
        }

        if (!err.empty()) {
            ARV_LOG_ERROR("ObjAssetRO error: {}", err);
        }

        if (!success) {
            ARV_LOG_ERROR("ObjAssetRO: Failed to load OBJ file: {}", objPath);
            return;
        }

        ARV_LOG_INFO("ObjAssetRO: Loaded {} shapes, {} materials", shapes.size(), materials.size());
        ARV_LOG_INFO("ObjAssetRO: {} vertices, {} normals, {} texcoords",
                     attrib.vertices.size() / 3,
                     attrib.normals.size() / 3,
                     attrib.texcoords.size() / 2);

        // Build interleaved vertex data: position (3) + texcoord (2) + normal (3) = 8 floats per vertex
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        // Use a map to avoid duplicate vertices
        std::unordered_map<std::string, uint32_t> uniqueVertices;

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                // Create a unique key for this vertex combination
                std::string key = std::to_string(index.vertex_index) + "/" +
                                  std::to_string(index.texcoord_index) + "/" +
                                  std::to_string(index.normal_index);

                if (uniqueVertices.count(key) == 0) {
                    // This is a new unique vertex
                    uniqueVertices[key] = static_cast<uint32_t>(vertices.size() / 8);

                    // Position
                    vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                    vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                    vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

                    // Texture coordinates (if available)
                    if (index.texcoord_index >= 0) {
                        vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                        vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
                    } else {
                        vertices.push_back(0.0f);
                        vertices.push_back(0.0f);
                    }

                    // Normal (if available)
                    if (index.normal_index >= 0) {
                        vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
                        vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
                        vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
                    } else {
                        vertices.push_back(0.0f);
                        vertices.push_back(1.0f);
                        vertices.push_back(0.0f);
                    }
                }

                indices.push_back(uniqueVertices[key]);
            }
        }

        ARV_LOG_INFO("ObjAssetRO: Built {} unique vertices, {} indices",
                     vertices.size() / 8, indices.size());

        // Shader with position, texcoord, and normal support
        std::string fullSource = R"(

            ### GLSL_VERTEX_SHADER ###

            #version 330 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec2 a_TexCoord;
            layout(location = 2) in vec3 a_Normal;

            uniform mat4 u_mvp;

            out vec2 v_TexCoord;
            out vec3 v_Normal;

            void main()
            {
                v_TexCoord = a_TexCoord;
                v_Normal = a_Normal;
                gl_Position = u_mvp * vec4(a_Position, 1.0);
            }

            ### GLSL_FRAGMENT_SHADER ###

            #version 330 core

            layout(location = 0) out vec4 color;

            in vec2 v_TexCoord;
            in vec3 v_Normal;

            uniform sampler2D u_Texture;

            void main()
            {
                // Simple directional lighting
                vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
                float diff = max(dot(normalize(v_Normal), lightDir), 0.3);
                vec4 texColor = texture(u_Texture, v_TexCoord);
                color = vec4(texColor.rgb * diff, texColor.a);
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
                float3 normal [[attribute(2)]];
            };

            struct VertexOut {
                float4 position [[position]];
                float2 texCoord;
                float3 normal;
            };

            vertex VertexOut vertexMain(VertexIn in [[stage_in]],
                                        constant VertexUniforms& uniforms [[buffer(1)]]) {
                VertexOut out;
                out.position = uniforms.u_mvp * float4(in.position, 1.0);
                out.texCoord = float2(in.texCoord.x, 1.0 - in.texCoord.y);
                out.normal = in.normal;
                return out;
            }

            fragment float4 fragmentMain(VertexOut in [[stage_in]],
                                         texture2d<float> tex [[texture(0)]],
                                         sampler texSampler [[sampler(0)]]) {
                float3 lightDir = normalize(float3(1.0, 1.0, 1.0));
                float diff = max(dot(normalize(in.normal), lightDir), 0.3);
                float4 texColor = tex.sample(texSampler, in.texCoord);
                return float4(texColor.rgb * diff, texColor.a);
            }
        )";

        m_ShaderSource = std::make_unique<CoreShaderSource>(fullSource);
        m_Shader = app->GetRenderer()->CreateShader(m_ShaderSource.get());
        m_Shader->Compile();

        m_VertexArray = app->GetRenderer()->CreateVertexArray();

        auto vertexBuffer = app->GetRenderer()->CreateVertexBuffer(vertices.data(),
                                                                    vertices.size() * sizeof(float));
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float3, "a_Normal" }
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        auto indexBuffer = app->GetRenderer()->CreateIndexBuffer(indices.data(),
                                                                  static_cast<unsigned int>(indices.size()));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_VertexArray->Unbind();

        // Load texture - try to find diffuse texture from materials
        std::string texturePath;
        if (!materials.empty() && !materials[0].diffuse_texname.empty()) {
            texturePath = m_AssetPath + "/" + materials[0].diffuse_texname;
            ARV_LOG_INFO("ObjAssetRO: Using material texture: {}", texturePath);
        } else {
            // Fallback: look for common texture naming convention
            texturePath = m_AssetPath + "/textures/" + pathFragment + "_Body_Mat_baseColor.png";
            ARV_LOG_INFO("ObjAssetRO: Using fallback texture: {}", texturePath);
        }

        m_Texture = app->GetRenderer()->CreateTexture2D(texturePath);
    }

    std::shared_ptr<Shader>& ObjAssetRO::GetShader() {
        return m_Shader;
    }

    std::shared_ptr<VertexArray>& ObjAssetRO::GetVertexArray() {
        return m_VertexArray;
    }

}
