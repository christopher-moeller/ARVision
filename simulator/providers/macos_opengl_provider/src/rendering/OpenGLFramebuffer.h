#pragma once

#include "rendering/Framebuffer.h"
#include <vector>

namespace arv {

    class OpenGLFramebuffer : public Framebuffer {
    public:
        OpenGLFramebuffer(const FramebufferSpecification& spec);
        ~OpenGLFramebuffer();

        void Bind() override;
        void Unbind() override;

        void Resize(uint32_t width, uint32_t height) override;

        uint32_t GetColorAttachmentID(uint32_t index = 0) const override;
        uint32_t GetWidth() const override { return m_Specification.width; }
        uint32_t GetHeight() const override { return m_Specification.height; }

        const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

    private:
        void Invalidate();

        uint32_t m_RendererID = 0;
        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
        FramebufferSpecification m_Specification;
    };

}
