#pragma once

#include "rendering/Framebuffer.h"
#include <vector>

#ifdef __OBJC__
@protocol MTLDevice;
@protocol MTLTexture;
@class MTLRenderPassDescriptor;
#else
typedef void MTLDevice;
typedef void MTLTexture;
typedef void MTLRenderPassDescriptor;
#endif

namespace arv {

    class MetalFramebuffer : public Framebuffer {
    public:
#ifdef __OBJC__
        MetalFramebuffer(id<MTLDevice> device, const FramebufferSpecification& spec);
#else
        MetalFramebuffer(void* device, const FramebufferSpecification& spec);
#endif
        ~MetalFramebuffer();

        void Bind() override;
        void Unbind() override;

        void Resize(uint32_t width, uint32_t height) override;

        uint32_t GetColorAttachmentID(uint32_t index = 0) const override;
        uint32_t GetWidth() const override { return m_Specification.width; }
        uint32_t GetHeight() const override { return m_Specification.height; }

        const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

#ifdef __OBJC__
        id<MTLTexture> GetColorTexture(uint32_t index = 0) const;
        id<MTLTexture> GetDepthTexture() const { return m_DepthTexture; }
        MTLRenderPassDescriptor* GetRenderPassDescriptor() const { return m_RenderPassDescriptor; }
#else
        void* GetColorTexture(uint32_t index = 0) const;
        void* GetDepthTexture() const { return m_DepthTexture; }
        void* GetRenderPassDescriptor() const { return m_RenderPassDescriptor; }
#endif

    private:
        void Invalidate();

#ifdef __OBJC__
        id<MTLDevice> m_Device = nullptr;
        std::vector<id<MTLTexture>> m_ColorTextures;
        id<MTLTexture> m_DepthTexture = nullptr;
        MTLRenderPassDescriptor* m_RenderPassDescriptor = nullptr;
#else
        void* m_Device = nullptr;
        std::vector<void*> m_ColorTextures;
        void* m_DepthTexture = nullptr;
        void* m_RenderPassDescriptor = nullptr;
#endif
        FramebufferSpecification m_Specification;
        bool m_IsBound = false;
    };

}
