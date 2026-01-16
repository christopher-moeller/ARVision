#include "MacosMetalRenderingAPI.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include "MetalBuffer.h"
#include "MetalShader.h"
#include "MetalVertexArray.h"

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
            const auto& mat4Uniforms = shader->GetMat4Uniforms();
            const auto& float4Uniforms = shader->GetFloat4Uniforms();

            // Bind vertex uniforms in the order defined by the shader's VertexUniforms struct
            if (!uniformLayout.vertexUniformNames.empty())
            {
                std::vector<float> vertexUniformData;
                vertexUniformData.reserve(uniformLayout.vertexUniformNames.size() * 16);

                for (const auto& fieldName : uniformLayout.vertexUniformNames)
                {
                    auto it = mat4Uniforms.find(fieldName);
                    if (it != mat4Uniforms.end())
                    {
                        const float* ptr = glm::value_ptr(it->second);
                        vertexUniformData.insert(vertexUniformData.end(), ptr, ptr + 16);
                    }
                }

                if (!vertexUniformData.empty())
                {
                    [renderEncoder setVertexBytes:vertexUniformData.data()
                                           length:vertexUniformData.size() * sizeof(float)
                                          atIndex:1];
                }
            }

            // Bind fragment uniforms in the order defined by the shader's FragmentUniforms struct
            if (!uniformLayout.fragmentUniformNames.empty())
            {
                std::vector<float> fragmentUniformData;
                fragmentUniformData.reserve(uniformLayout.fragmentUniformNames.size() * 4);

                for (const auto& fieldName : uniformLayout.fragmentUniformNames)
                {
                    auto it = float4Uniforms.find(fieldName);
                    if (it != float4Uniforms.end())
                    {
                        fragmentUniformData.push_back(it->second.x);
                        fragmentUniformData.push_back(it->second.y);
                        fragmentUniformData.push_back(it->second.z);
                        fragmentUniformData.push_back(it->second.w);
                    }
                }

                if (!fragmentUniformData.empty())
                {
                    [renderEncoder setFragmentBytes:fragmentUniformData.data()
                                             length:fragmentUniformData.size() * sizeof(float)
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
}
