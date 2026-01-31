#include "MetalFramebuffer.h"
#include "ARVBase.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

namespace arv {

    static MTLPixelFormat TextureFormatToMetal(FramebufferTextureFormat format)
    {
        switch (format)
        {
            // Use BGRA8Unorm to match the screen drawable format (CAMetalLayer default)
            case FramebufferTextureFormat::RGBA8:    return MTLPixelFormatBGRA8Unorm;
            case FramebufferTextureFormat::RGBA16F:  return MTLPixelFormatRGBA16Float;
            case FramebufferTextureFormat::RGBA32F:  return MTLPixelFormatRGBA32Float;
            default: return MTLPixelFormatBGRA8Unorm;
        }
    }

    MetalFramebuffer::MetalFramebuffer(id<MTLDevice> device, const FramebufferSpecification& spec)
        : m_Device(device), m_Specification(spec)
    {
        ARV_LOG_INFO("MetalFramebuffer::MetalFramebuffer() - Creating framebuffer {}x{} with {} color attachments",
                     spec.width, spec.height, spec.colorAttachments.size());
        Invalidate();
    }

    MetalFramebuffer::~MetalFramebuffer()
    {
        ARV_LOG_INFO("MetalFramebuffer::~MetalFramebuffer() - Destroying framebuffer {}x{}",
                     m_Specification.width, m_Specification.height);
        m_ColorTextures.clear();
        m_DepthTexture = nil;
        m_RenderPassDescriptor = nil;
    }

    void MetalFramebuffer::Invalidate()
    {
        ARV_LOG_INFO("MetalFramebuffer::Invalidate() - Recreating framebuffer textures");
        m_ColorTextures.clear();
        m_DepthTexture = nil;
        m_RenderPassDescriptor = nil;

        ARV_LOG_INFO("MetalFramebuffer::Invalidate() - Creating {} color attachments", m_Specification.colorAttachments.size());
        for (size_t i = 0; i < m_Specification.colorAttachments.size(); i++)
        {
            MTLTextureDescriptor* colorDesc = [MTLTextureDescriptor
                texture2DDescriptorWithPixelFormat:TextureFormatToMetal(m_Specification.colorAttachments[i])
                width:m_Specification.width
                height:m_Specification.height
                mipmapped:NO];
            colorDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
            colorDesc.storageMode = MTLStorageModePrivate;

            id<MTLTexture> colorTexture = [m_Device newTextureWithDescriptor:colorDesc];
            m_ColorTextures.push_back(colorTexture);
        }

        // Create depth attachment if needed
        if (m_Specification.hasDepthAttachment)
        {
            MTLTextureDescriptor* depthDesc = [MTLTextureDescriptor
                texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                width:m_Specification.width
                height:m_Specification.height
                mipmapped:NO];
            depthDesc.usage = MTLTextureUsageRenderTarget;
            depthDesc.storageMode = MTLStorageModePrivate;

            m_DepthTexture = [m_Device newTextureWithDescriptor:depthDesc];
        }

        // Create render pass descriptor
        m_RenderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

        // Configure color attachments
        for (size_t i = 0; i < m_ColorTextures.size(); i++)
        {
            m_RenderPassDescriptor.colorAttachments[i].texture = m_ColorTextures[i];
            m_RenderPassDescriptor.colorAttachments[i].loadAction = MTLLoadActionClear;
            m_RenderPassDescriptor.colorAttachments[i].storeAction = MTLStoreActionStore;
            m_RenderPassDescriptor.colorAttachments[i].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
        }

        // Configure depth attachment
        if (m_DepthTexture)
        {
            m_RenderPassDescriptor.depthAttachment.texture = m_DepthTexture;
            m_RenderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
            m_RenderPassDescriptor.depthAttachment.storeAction = MTLStoreActionDontCare;
            m_RenderPassDescriptor.depthAttachment.clearDepth = 1.0;
        }
    }

    void MetalFramebuffer::Bind()
    {
        ARV_LOG_INFO("MetalFramebuffer::Bind() - Binding framebuffer");
        m_IsBound = true;
    }

    void MetalFramebuffer::Unbind()
    {
        ARV_LOG_INFO("MetalFramebuffer::Unbind() - Unbinding framebuffer");
        m_IsBound = false;
    }

    void MetalFramebuffer::Resize(uint32_t width, uint32_t height)
    {
        ARV_LOG_INFO("MetalFramebuffer::Resize() - Resizing from {}x{} to {}x{}",
                     m_Specification.width, m_Specification.height, width, height);
        if (width == 0 || height == 0 || width > 8192 || height > 8192)
        {
            ARV_LOG_ERROR("MetalFramebuffer::Resize() - Invalid dimensions: {}x{}", width, height);
            return;
        }

        m_Specification.width = width;
        m_Specification.height = height;
        Invalidate();
    }

    uint32_t MetalFramebuffer::GetColorAttachmentID(uint32_t index) const
    {
        // Metal doesn't use integer IDs like OpenGL
        // Return the index for identification purposes
        if (index < m_ColorTextures.size())
        {
            return index;
        }
        return 0;
    }

    id<MTLTexture> MetalFramebuffer::GetColorTexture(uint32_t index) const
    {
        if (index < m_ColorTextures.size())
        {
            return m_ColorTextures[index];
        }
        return nil;
    }

}
