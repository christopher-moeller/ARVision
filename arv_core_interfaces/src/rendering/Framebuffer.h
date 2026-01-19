#pragma once

#include <cstdint>
#include <vector>

namespace arv {

    enum class FramebufferTextureFormat {
        None = 0,

        // Color formats
        RGBA8,
        RGBA16F,
        RGBA32F,

        // Depth formats
        Depth24Stencil8,
        Depth32F
    };

    struct FramebufferSpecification {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t samples = 1;
        std::vector<FramebufferTextureFormat> colorAttachments;
        bool hasDepthAttachment = true;
    };

    class Framebuffer {
    public:
        virtual ~Framebuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual uint32_t GetColorAttachmentID(uint32_t index = 0) const = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual const FramebufferSpecification& GetSpecification() const = 0;
    };

}
