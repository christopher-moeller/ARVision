#include "MacosMetalRenderingAPI.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include "MetalBuffer.h"
#include "MetalShader.h"
#include "MetalVertexArray.h"
#include "MetalTexture.h"

#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace arv
{
    MacosMetalRenderingAPI::MacosMetalRenderingAPI()
    {
    }

    MacosMetalRenderingAPI::~MacosMetalRenderingAPI()
    {
        m_commandQueue = nil;
        m_device = nil;
    }

    void MacosMetalRenderingAPI::SetMetalLayer(CAMetalLayer* layer)
    {
        m_metalLayer = layer;
    }

    void MacosMetalRenderingAPI::Init(PlattformApplicationContext* context)
    {
        if (!m_metalLayer)
        {
            std::cerr << "Metal layer not set!" << std::endl;
            return;
        }

        m_device = m_metalLayer.device;
        if (!m_device)
        {
            std::cerr << "Failed to get Metal device from layer!" << std::endl;
            return;
        }

        m_commandQueue = [m_device newCommandQueue];
        if (!m_commandQueue)
        {
            std::cerr << "Failed to create Metal command queue!" << std::endl;
            return;
        }

        std::cout << "MacosMetalRenderingAPI::Init() - Metal initialized successfully" << std::endl;
    }

    void MacosMetalRenderingAPI::DrawExample()
    {
        // Example drawing - just clears the screen
        Clear();
    }

    void MacosMetalRenderingAPI::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
    {
        if (!m_metalLayer || !m_commandQueue)
        {
            return;
        }

        @autoreleasepool {
            // Get the next drawable
            id<CAMetalDrawable> drawable = [m_metalLayer nextDrawable];
            if (!drawable)
            {
                return;
            }

            // Create render pass descriptor
            MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
            renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(
                m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

            // Create command buffer
            id<MTLCommandBuffer> commandBuffer = [m_commandQueue commandBuffer];
            if (!commandBuffer)
            {
                return;
            }

            // Create render encoder
            id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
            if (!renderEncoder)
            {
                return;
            }

            // Get Metal-specific objects
            MetalShader* metalShader = static_cast<MetalShader*>(shader.get());
            MetalVertexArray* metalVA = static_cast<MetalVertexArray*>(vertexArray.get());

            // Set the render pipeline state
            if (metalShader && metalShader->GetPipelineState())
            {
                [renderEncoder setRenderPipelineState:metalShader->GetPipelineState()];
            }

            // Bind vertex buffers
            const auto& vertexBuffers = metalVA->GetVertexBuffers();
            for (size_t i = 0; i < vertexBuffers.size(); ++i)
            {
                MetalVertexBuffer* metalVB = static_cast<MetalVertexBuffer*>(vertexBuffers[i].get());
                if (metalVB && metalVB->GetMetalBuffer())
                {
                    [renderEncoder setVertexBuffer:metalVB->GetMetalBuffer() offset:0 atIndex:i];
                }
            }

            // Get the uniform layout parsed from the shader source
            const auto& uniformLayout = metalShader->GetUniformLayout();

            // Helper lambda to pack uniform data based on field type
            auto packUniformData = [&shader](const std::vector<UniformField>& fields) -> std::vector<uint8_t> {
                std::vector<uint8_t> data;

                const auto& intUniforms = shader->GetIntUniforms();
                const auto& floatUniforms = shader->GetFloatUniforms();
                const auto& float2Uniforms = shader->GetFloat2Uniforms();
                const auto& float3Uniforms = shader->GetFloat3Uniforms();
                const auto& float4Uniforms = shader->GetFloat4Uniforms();
                const auto& mat3Uniforms = shader->GetMat3Uniforms();
                const auto& mat4Uniforms = shader->GetMat4Uniforms();

                for (const auto& field : fields)
                {
                    switch (field.type)
                    {
                        case MetalDataType::Int:
                        {
                            auto it = intUniforms.find(field.name);
                            if (it != intUniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&it->second);
                                data.insert(data.end(), ptr, ptr + sizeof(int));
                            }
                            break;
                        }
                        case MetalDataType::Float:
                        {
                            auto it = floatUniforms.find(field.name);
                            if (it != floatUniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&it->second);
                                data.insert(data.end(), ptr, ptr + sizeof(float));
                            }
                            break;
                        }
                        case MetalDataType::Float2:
                        {
                            auto it = float2Uniforms.find(field.name);
                            if (it != float2Uniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(it->second));
                                data.insert(data.end(), ptr, ptr + sizeof(glm::vec2));
                            }
                            break;
                        }
                        case MetalDataType::Float3:
                        {
                            auto it = float3Uniforms.find(field.name);
                            if (it != float3Uniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(it->second));
                                data.insert(data.end(), ptr, ptr + sizeof(glm::vec3));
                                // Add padding to align to 16 bytes (Metal requires float3 to be padded)
                                data.insert(data.end(), sizeof(float), 0);
                            }
                            break;
                        }
                        case MetalDataType::Float4:
                        {
                            auto it = float4Uniforms.find(field.name);
                            if (it != float4Uniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(it->second));
                                data.insert(data.end(), ptr, ptr + sizeof(glm::vec4));
                            }
                            break;
                        }
                        case MetalDataType::Mat3:
                        {
                            auto it = mat3Uniforms.find(field.name);
                            if (it != mat3Uniforms.end())
                            {
                                // Metal float3x3 is stored as 3 float4 columns (padded)
                                const glm::mat3& mat = it->second;
                                for (int col = 0; col < 3; ++col)
                                {
                                    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(mat[col]));
                                    data.insert(data.end(), ptr, ptr + sizeof(glm::vec3));
                                    // Pad each column to 16 bytes
                                    data.insert(data.end(), sizeof(float), 0);
                                }
                            }
                            break;
                        }
                        case MetalDataType::Mat4:
                        {
                            auto it = mat4Uniforms.find(field.name);
                            if (it != mat4Uniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(it->second));
                                data.insert(data.end(), ptr, ptr + sizeof(glm::mat4));
                            }
                            break;
                        }
                    }
                }

                return data;
            };

            // Bind vertex uniforms in the order defined by the shader's VertexUniforms struct
            if (!uniformLayout.vertexUniforms.empty())
            {
                std::vector<uint8_t> vertexUniformData = packUniformData(uniformLayout.vertexUniforms);

                if (!vertexUniformData.empty())
                {
                    [renderEncoder setVertexBytes:vertexUniformData.data()
                                           length:vertexUniformData.size()
                                          atIndex:1];
                }
            }

            // Bind fragment uniforms in the order defined by the shader's FragmentUniforms struct
            if (!uniformLayout.fragmentUniforms.empty())
            {
                std::vector<uint8_t> fragmentUniformData = packUniformData(uniformLayout.fragmentUniforms);

                if (!fragmentUniformData.empty())
                {
                    [renderEncoder setFragmentBytes:fragmentUniformData.data()
                                             length:fragmentUniformData.size()
                                            atIndex:0];
                }
            }

            // Get index buffer
            const auto& indexBuffer = metalVA->GetIndexBuffer();
            if (indexBuffer)
            {
                MetalIndexBuffer* metalIB = static_cast<MetalIndexBuffer*>(indexBuffer.get());
                if (metalIB && metalIB->GetMetalBuffer())
                {
                    [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                              indexCount:metalIB->GetCount()
                                               indexType:MTLIndexTypeUInt32
                                             indexBuffer:metalIB->GetMetalBuffer()
                                       indexBufferOffset:0];
                }
            }

            [renderEncoder endEncoding];

            // Present and commit
            [commandBuffer presentDrawable:drawable];
            [commandBuffer commit];
        }
    }

    void MacosMetalRenderingAPI::SetClearColor(const glm::vec4& color)
    {
        m_clearColor = color;
    }

    void MacosMetalRenderingAPI::Clear()
    {
        // In Metal, clearing is handled by Draw() via MTLLoadActionClear
        // SetClearColor() stores the color, and Draw() uses it in the render pass descriptor
        // This is a no-op to avoid consuming a drawable before Draw() is called
    }

    std::shared_ptr<VertexBuffer> MacosMetalRenderingAPI::CreateVertexBuffer(float* vertices, unsigned int size)
    {
        return std::make_shared<MetalVertexBuffer>(m_device, vertices, size);
    }

    std::shared_ptr<IndexBuffer> MacosMetalRenderingAPI::CreateIndexBuffer(unsigned int* indices, unsigned int size)
    {
        return std::make_shared<MetalIndexBuffer>(m_device, indices, size);
    }

    std::shared_ptr<VertexArray> MacosMetalRenderingAPI::CreateVertexArray()
    {
        return std::make_shared<MetalVertexArray>();
    }

    std::shared_ptr<Shader> MacosMetalRenderingAPI::CreateShader(ShaderSource* shaderSource)
    {
        return std::make_shared<MetalShader>(m_device, shaderSource);
    }

    std::shared_ptr<Texture2D> MacosMetalRenderingAPI::CreateTexture2D(const std::string& path)
    {
        return std::make_shared<MetalTexture2D>(m_device, path);
    }

    void MacosMetalRenderingAPI::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture)
    {
        if (!m_metalLayer || !m_commandQueue)
        {
            return;
        }

        @autoreleasepool {
            // Get the next drawable
            id<CAMetalDrawable> drawable = [m_metalLayer nextDrawable];
            if (!drawable)
            {
                return;
            }

            // Create render pass descriptor
            MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
            renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(
                m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

            // Create command buffer
            id<MTLCommandBuffer> commandBuffer = [m_commandQueue commandBuffer];
            if (!commandBuffer)
            {
                return;
            }

            // Create render encoder
            id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
            if (!renderEncoder)
            {
                return;
            }

            // Get Metal-specific objects
            MetalShader* metalShader = static_cast<MetalShader*>(shader.get());
            MetalVertexArray* metalVA = static_cast<MetalVertexArray*>(vertexArray.get());

            // Set the render pipeline state
            if (metalShader && metalShader->GetPipelineState())
            {
                [renderEncoder setRenderPipelineState:metalShader->GetPipelineState()];
            }

            // Bind vertex buffers
            const auto& vertexBuffers = metalVA->GetVertexBuffers();
            for (size_t i = 0; i < vertexBuffers.size(); ++i)
            {
                MetalVertexBuffer* metalVB = static_cast<MetalVertexBuffer*>(vertexBuffers[i].get());
                if (metalVB && metalVB->GetMetalBuffer())
                {
                    [renderEncoder setVertexBuffer:metalVB->GetMetalBuffer() offset:0 atIndex:i];
                }
            }

            // Get the uniform layout parsed from the shader source
            const auto& uniformLayout = metalShader->GetUniformLayout();

            // Helper lambda to pack uniform data based on field type
            auto packUniformData = [&shader](const std::vector<UniformField>& fields) -> std::vector<uint8_t> {
                std::vector<uint8_t> data;

                const auto& intUniforms = shader->GetIntUniforms();
                const auto& floatUniforms = shader->GetFloatUniforms();
                const auto& float2Uniforms = shader->GetFloat2Uniforms();
                const auto& float3Uniforms = shader->GetFloat3Uniforms();
                const auto& float4Uniforms = shader->GetFloat4Uniforms();
                const auto& mat3Uniforms = shader->GetMat3Uniforms();
                const auto& mat4Uniforms = shader->GetMat4Uniforms();

                for (const auto& field : fields)
                {
                    switch (field.type)
                    {
                        case MetalDataType::Int:
                        {
                            auto it = intUniforms.find(field.name);
                            if (it != intUniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&it->second);
                                data.insert(data.end(), ptr, ptr + sizeof(int));
                            }
                            break;
                        }
                        case MetalDataType::Float:
                        {
                            auto it = floatUniforms.find(field.name);
                            if (it != floatUniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&it->second);
                                data.insert(data.end(), ptr, ptr + sizeof(float));
                            }
                            break;
                        }
                        case MetalDataType::Float2:
                        {
                            auto it = float2Uniforms.find(field.name);
                            if (it != float2Uniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(it->second));
                                data.insert(data.end(), ptr, ptr + sizeof(glm::vec2));
                            }
                            break;
                        }
                        case MetalDataType::Float3:
                        {
                            auto it = float3Uniforms.find(field.name);
                            if (it != float3Uniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(it->second));
                                data.insert(data.end(), ptr, ptr + sizeof(glm::vec3));
                                // Add padding to align to 16 bytes (Metal requires float3 to be padded)
                                data.insert(data.end(), sizeof(float), 0);
                            }
                            break;
                        }
                        case MetalDataType::Float4:
                        {
                            auto it = float4Uniforms.find(field.name);
                            if (it != float4Uniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(it->second));
                                data.insert(data.end(), ptr, ptr + sizeof(glm::vec4));
                            }
                            break;
                        }
                        case MetalDataType::Mat3:
                        {
                            auto it = mat3Uniforms.find(field.name);
                            if (it != mat3Uniforms.end())
                            {
                                // Metal float3x3 is stored as 3 float4 columns (padded)
                                const glm::mat3& mat = it->second;
                                for (int col = 0; col < 3; ++col)
                                {
                                    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(mat[col]));
                                    data.insert(data.end(), ptr, ptr + sizeof(glm::vec3));
                                    // Pad each column to 16 bytes
                                    data.insert(data.end(), sizeof(float), 0);
                                }
                            }
                            break;
                        }
                        case MetalDataType::Mat4:
                        {
                            auto it = mat4Uniforms.find(field.name);
                            if (it != mat4Uniforms.end())
                            {
                                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(glm::value_ptr(it->second));
                                data.insert(data.end(), ptr, ptr + sizeof(glm::mat4));
                            }
                            break;
                        }
                    }
                }

                return data;
            };

            // Bind vertex uniforms
            if (!uniformLayout.vertexUniforms.empty())
            {
                std::vector<uint8_t> vertexUniformData = packUniformData(uniformLayout.vertexUniforms);

                if (!vertexUniformData.empty())
                {
                    [renderEncoder setVertexBytes:vertexUniformData.data()
                                           length:vertexUniformData.size()
                                          atIndex:1];
                }
            }

            // Bind texture if provided
            if (texture)
            {
                MetalTexture2D* metalTex = static_cast<MetalTexture2D*>(texture.get());
                if (metalTex && metalTex->GetMetalTexture())
                {
                    [renderEncoder setFragmentTexture:metalTex->GetMetalTexture() atIndex:0];
                    [renderEncoder setFragmentSamplerState:metalTex->GetSamplerState() atIndex:0];
                }
            }

            // Get index buffer
            const auto& indexBuffer = metalVA->GetIndexBuffer();
            if (indexBuffer)
            {
                MetalIndexBuffer* metalIB = static_cast<MetalIndexBuffer*>(indexBuffer.get());
                if (metalIB && metalIB->GetMetalBuffer())
                {
                    [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                              indexCount:metalIB->GetCount()
                                               indexType:MTLIndexTypeUInt32
                                             indexBuffer:metalIB->GetMetalBuffer()
                                       indexBufferOffset:0];
                }
            }

            [renderEncoder endEncoding];

            // Present and commit
            [commandBuffer presentDrawable:drawable];
            [commandBuffer commit];
        }
    }
}
