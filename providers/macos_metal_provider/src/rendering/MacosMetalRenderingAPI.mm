#include "MacosMetalRenderingAPI.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include "MetalBuffer.h"
#include "MetalShader.h"
#include "MetalVertexArray.h"
#include "MetalTexture.h"
#include "MetalHDRTexture.h"
#include "MetalFramebuffer.h"

#include "ARVBase.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace arv
{
    MacosMetalRenderingAPI::MacosMetalRenderingAPI()
    {
    }

    MacosMetalRenderingAPI::~MacosMetalRenderingAPI()
    {
        m_depthTexture = nil;
        m_depthStencilState = nil;
        m_commandQueue = nil;
        m_device = nil;
    }

    void MacosMetalRenderingAPI::SetMetalLayer(CAMetalLayer* layer)
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::SetMetalLayer() - Metal layer set");
        m_metalLayer = layer;
    }

    void MacosMetalRenderingAPI::Init(PlatformApplicationContext* context)
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::Init() - Starting initialization");

        if (!m_metalLayer)
        {
            ARV_LOG_ERROR("MacosMetalRenderingAPI::Init() - Metal layer not set!");
            return;
        }

        m_device = m_metalLayer.device;
        if (!m_device)
        {
            ARV_LOG_ERROR("MacosMetalRenderingAPI::Init() - Failed to get Metal device from layer!");
            return;
        }
        ARV_LOG_INFO("MacosMetalRenderingAPI::Init() - Got Metal device: {}", [[m_device name] UTF8String]);

        m_commandQueue = [m_device newCommandQueue];
        if (!m_commandQueue)
        {
            ARV_LOG_ERROR("MacosMetalRenderingAPI::Init() - Failed to create Metal command queue!");
            return;
        }
        ARV_LOG_INFO("MacosMetalRenderingAPI::Init() - Command queue created");

        ARV_LOG_INFO("MacosMetalRenderingAPI::Init() - Creating depth stencil state");
        MTLDepthStencilDescriptor* depthDescriptor = [[MTLDepthStencilDescriptor alloc] init];
        depthDescriptor.depthCompareFunction = MTLCompareFunctionLess;
        depthDescriptor.depthWriteEnabled = YES;
        m_depthStencilState = [m_device newDepthStencilStateWithDescriptor:depthDescriptor];

        ARV_LOG_INFO("MacosMetalRenderingAPI::Init() - Metal initialized successfully");
    }

    void MacosMetalRenderingAPI::DrawExample()
    {
        // Example drawing - just clears the screen
        Clear();
    }

    void MacosMetalRenderingAPI::CreateDepthTextureIfNeeded(size_t width, size_t height)
    {
        // Check if we need to create or recreate the depth texture
        if (m_depthTexture)
        {
            if ([m_depthTexture width] == width && [m_depthTexture height] == height)
            {
                return; // Depth texture already exists with correct size
            }
        }

        // Create depth texture descriptor
        MTLTextureDescriptor* depthTextureDescriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
            width:width
            height:height
            mipmapped:NO];
        depthTextureDescriptor.usage = MTLTextureUsageRenderTarget;
        depthTextureDescriptor.storageMode = MTLStorageModePrivate;

        m_depthTexture = [m_device newTextureWithDescriptor:depthTextureDescriptor];
    }

    void MacosMetalRenderingAPI::BeginFrame()
    {
        if (!m_metalLayer || !m_commandQueue || m_frameInProgress)
        {
            ARV_LOG_INFO("BeginFrame: early return - metalLayer:{} commandQueue:{} frameInProgress:{}",
                (m_metalLayer != nil), (m_commandQueue != nil), m_frameInProgress);
            return;
        }

        // Get the next drawable (needed even for off-screen rendering for eventual presentation)
        m_currentDrawable = [m_metalLayer nextDrawable];
        if (!m_currentDrawable)
        {
            ARV_LOG_ERROR("BeginFrame: Failed to get next drawable");
            return;
        }

        MTLRenderPassDescriptor* renderPassDescriptor;

        if (m_boundFramebuffer)
        {
            // Use the bound framebuffer's render pass descriptor
            MetalFramebuffer* metalFB = static_cast<MetalFramebuffer*>(m_boundFramebuffer.get());
            renderPassDescriptor = metalFB->GetRenderPassDescriptor();
        }
        else
        {
            // Use default screen framebuffer
            // Create or update depth texture to match drawable size
            size_t width = m_currentDrawable.texture.width;
            size_t height = m_currentDrawable.texture.height;
            CreateDepthTextureIfNeeded(width, height);

            // Create render pass descriptor with clear action
            renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
            renderPassDescriptor.colorAttachments[0].texture = m_currentDrawable.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(
                m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

            // Configure depth attachment
            renderPassDescriptor.depthAttachment.texture = m_depthTexture;
            renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
            renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionDontCare;
            renderPassDescriptor.depthAttachment.clearDepth = 1.0;
        }

        // Create command buffer
        m_currentCommandBuffer = [m_commandQueue commandBuffer];
        if (!m_currentCommandBuffer)
        {
            m_currentDrawable = nil;
            return;
        }

        // Create render encoder
        m_currentRenderEncoder = [m_currentCommandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        if (!m_currentRenderEncoder)
        {
            m_currentCommandBuffer = nil;
            m_currentDrawable = nil;
            return;
        }

        // Set depth stencil state for depth testing
        [m_currentRenderEncoder setDepthStencilState:m_depthStencilState];

        m_frameInProgress = true;
    }

    void MacosMetalRenderingAPI::EndFrame()
    {
        if (!m_frameInProgress || !m_currentCommandBuffer || !m_currentDrawable)
        {
            return;
        }

        // End encoding if still active (might have been ended by ImGui layer)
        if (m_currentRenderEncoder)
        {
            [m_currentRenderEncoder endEncoding];
            m_currentRenderEncoder = nil;
        }

        // Present and commit
        [m_currentCommandBuffer presentDrawable:m_currentDrawable];
        [m_currentCommandBuffer commit];

        // Reset frame state
        m_currentCommandBuffer = nil;
        m_currentDrawable = nil;
        m_frameInProgress = false;
    }

    void MacosMetalRenderingAPI::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
    {
        DrawInternal(shader, vertexArray, nullptr);
    }

    void MacosMetalRenderingAPI::DrawInternal(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture)
    {
        if (!m_frameInProgress || !m_currentRenderEncoder)
        {
            return;
        }

        // Get Metal-specific objects
        MetalShader* metalShader = static_cast<MetalShader*>(shader.get());
        MetalVertexArray* metalVA = static_cast<MetalVertexArray*>(vertexArray.get());

        // Set the render pipeline state
        if (metalShader && metalShader->GetPipelineState())
        {
            [m_currentRenderEncoder setRenderPipelineState:metalShader->GetPipelineState()];
        }

        // Bind vertex buffers
        const auto& vertexBuffers = metalVA->GetVertexBuffers();
        for (size_t i = 0; i < vertexBuffers.size(); ++i)
        {
            MetalVertexBuffer* metalVB = static_cast<MetalVertexBuffer*>(vertexBuffers[i].get());
            if (metalVB && metalVB->GetMetalBuffer())
            {
                [m_currentRenderEncoder setVertexBuffer:metalVB->GetMetalBuffer() offset:0 atIndex:i];
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
                [m_currentRenderEncoder setVertexBytes:vertexUniformData.data()
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
                [m_currentRenderEncoder setFragmentBytes:fragmentUniformData.data()
                                         length:fragmentUniformData.size()
                                        atIndex:0];
            }
        }

        // Bind texture if provided
        if (texture)
        {
            MetalTexture2D* metalTex = static_cast<MetalTexture2D*>(texture.get());
            if (metalTex && metalTex->GetMetalTexture())
            {
                [m_currentRenderEncoder setFragmentTexture:metalTex->GetMetalTexture() atIndex:0];
                [m_currentRenderEncoder setFragmentSamplerState:metalTex->GetSamplerState() atIndex:0];
            }
        }

        // Get index buffer
        const auto& indexBuffer = metalVA->GetIndexBuffer();
        if (indexBuffer)
        {
            MetalIndexBuffer* metalIB = static_cast<MetalIndexBuffer*>(indexBuffer.get());
            if (metalIB && metalIB->GetMetalBuffer())
            {
                [m_currentRenderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                          indexCount:metalIB->GetCount()
                                           indexType:MTLIndexTypeUInt32
                                         indexBuffer:metalIB->GetMetalBuffer()
                                   indexBufferOffset:0];
            }
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
        ARV_LOG_INFO("MacosMetalRenderingAPI::CreateVertexBuffer() - Creating vertex buffer with {} bytes", size);
        return std::make_shared<MetalVertexBuffer>(m_device, vertices, size);
    }

    std::shared_ptr<IndexBuffer> MacosMetalRenderingAPI::CreateIndexBuffer(unsigned int* indices, unsigned int size)
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::CreateIndexBuffer() - Creating index buffer with {} indices", size);
        return std::make_shared<MetalIndexBuffer>(m_device, indices, size);
    }

    std::shared_ptr<VertexArray> MacosMetalRenderingAPI::CreateVertexArray()
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::CreateVertexArray() - Creating vertex array");
        return std::make_shared<MetalVertexArray>();
    }

    std::shared_ptr<Shader> MacosMetalRenderingAPI::CreateShader(ShaderSource* shaderSource)
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::CreateShader() - Creating shader from source");
        return std::make_shared<MetalShader>(m_device, shaderSource);
    }

    std::shared_ptr<Texture2D> MacosMetalRenderingAPI::CreateTexture2D(const std::string& path)
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::CreateTexture2D() - Creating texture from path: {}", path);
        return std::make_shared<MetalTexture2D>(m_device, path);
    }

    std::shared_ptr<Texture2D> MacosMetalRenderingAPI::CreateHDRTexture2D(const std::string& path)
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::CreateHDRTexture2D() - Creating HDR texture from path: {}", path);
        return std::make_shared<MetalHDRTexture2D>(m_device, path);
    }

    void MacosMetalRenderingAPI::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture)
    {
        DrawInternal(shader, vertexArray, texture);
    }

    std::shared_ptr<Framebuffer> MacosMetalRenderingAPI::CreateFramebuffer(const FramebufferSpecification& spec)
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::CreateFramebuffer() - Creating framebuffer {}x{}", spec.width, spec.height);
        return std::make_shared<MetalFramebuffer>(m_device, spec);
    }

    void MacosMetalRenderingAPI::BindFramebuffer(const std::shared_ptr<Framebuffer>& framebuffer)
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::BindFramebuffer() - Binding framebuffer");
        m_boundFramebuffer = framebuffer;
        if (m_boundFramebuffer)
        {
            m_boundFramebuffer->Bind();
        }
    }

    void MacosMetalRenderingAPI::UnbindFramebuffer()
    {
        ARV_LOG_INFO("MacosMetalRenderingAPI::UnbindFramebuffer() - Unbinding framebuffer");
        if (m_boundFramebuffer)
        {
            m_boundFramebuffer->Unbind();
        }
        m_boundFramebuffer = nullptr;
    }

    void MacosMetalRenderingAPI::EndRenderPass()
    {
        if (m_currentRenderEncoder)
        {
            [m_currentRenderEncoder endEncoding];
            m_currentRenderEncoder = nil;
        }
    }

    MTLRenderPassDescriptor* MacosMetalRenderingAPI::CreateImGuiRenderPassDescriptor()
    {
        if (!m_currentDrawable)
        {
            ARV_LOG_ERROR("CreateImGuiRenderPassDescriptor: m_currentDrawable is nil");
            return nil;
        }

        if (!m_frameInProgress)
        {
            ARV_LOG_ERROR("CreateImGuiRenderPassDescriptor: frame not in progress");
            return nil;
        }

        MTLRenderPassDescriptor* descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        descriptor.colorAttachments[0].texture = m_currentDrawable.texture;
        descriptor.colorAttachments[0].loadAction = MTLLoadActionLoad; // Preserve existing content
        descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

        return descriptor;
    }

    id<MTLRenderCommandEncoder> MacosMetalRenderingAPI::CreateImGuiRenderEncoder(MTLRenderPassDescriptor* descriptor)
    {
        if (!m_currentCommandBuffer || !descriptor)
        {
            return nil;
        }

        return [m_currentCommandBuffer renderCommandEncoderWithDescriptor:descriptor];
    }

    void MacosMetalRenderingAPI::EndImGuiRenderPass(id<MTLRenderCommandEncoder> encoder)
    {
        if (encoder)
        {
            [encoder endEncoding];
        }
    }

    void MacosMetalRenderingAPI::BeginFramebufferPass(const std::shared_ptr<Framebuffer>& framebuffer, const glm::vec4& clearColor)
    {
        if (!m_currentCommandBuffer)
        {
            ARV_LOG_ERROR("BeginFramebufferPass: No current command buffer");
            return;
        }

        // End any existing render pass
        if (m_currentRenderEncoder)
        {
            [m_currentRenderEncoder endEncoding];
            m_currentRenderEncoder = nil;
        }

        // Get the framebuffer's render pass descriptor
        MetalFramebuffer* metalFB = static_cast<MetalFramebuffer*>(framebuffer.get());
        MTLRenderPassDescriptor* fbDescriptor = metalFB->GetRenderPassDescriptor();

        // Update clear color
        fbDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(
            clearColor.r, clearColor.g, clearColor.b, clearColor.a);

        // Create render encoder for the framebuffer
        m_currentRenderEncoder = [m_currentCommandBuffer renderCommandEncoderWithDescriptor:fbDescriptor];
        if (!m_currentRenderEncoder)
        {
            ARV_LOG_ERROR("BeginFramebufferPass: Failed to create render encoder");
            return;
        }

        // Set depth stencil state
        [m_currentRenderEncoder setDepthStencilState:m_depthStencilState];

        m_boundFramebuffer = framebuffer;
    }

    void MacosMetalRenderingAPI::EndFramebufferPass()
    {
        if (m_currentRenderEncoder)
        {
            [m_currentRenderEncoder endEncoding];
            m_currentRenderEncoder = nil;
        }
        m_boundFramebuffer = nullptr;
    }
}
