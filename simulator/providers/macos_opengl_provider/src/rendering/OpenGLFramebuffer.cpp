#include "OpenGLFramebuffer.h"
#include "ARVBase.h"
#include <glad/glad.h>

namespace arv {

    static GLenum TextureFormatToGL(FramebufferTextureFormat format)
    {
        switch (format)
        {
            case FramebufferTextureFormat::RGBA8:    return GL_RGBA8;
            case FramebufferTextureFormat::RGBA16F:  return GL_RGBA16F;
            case FramebufferTextureFormat::RGBA32F:  return GL_RGBA32F;
            default: return GL_RGBA8;
        }
    }

    OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
        : m_Specification(spec)
    {
        ARV_LOG_INFO("OpenGLFramebuffer::OpenGLFramebuffer() - Creating framebuffer {}x{} with {} color attachments",
                     spec.width, spec.height, spec.colorAttachments.size());
        Invalidate();
    }

    OpenGLFramebuffer::~OpenGLFramebuffer()
    {
        ARV_LOG_INFO("OpenGLFramebuffer::~OpenGLFramebuffer() - Destroying framebuffer {}x{}",
                     m_Specification.width, m_Specification.height);
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());
        if (m_DepthAttachment)
        {
            glDeleteTextures(1, &m_DepthAttachment);
        }
    }

    void OpenGLFramebuffer::Invalidate()
    {
        ARV_LOG_INFO("OpenGLFramebuffer::Invalidate() - Recreating framebuffer textures");
        if (m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());
            if (m_DepthAttachment)
            {
                glDeleteTextures(1, &m_DepthAttachment);
            }
            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
        }

        ARV_LOG_INFO("OpenGLFramebuffer::Invalidate() - Creating framebuffer");
        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        // Create color attachments
        if (!m_Specification.colorAttachments.empty())
        {
            m_ColorAttachments.resize(m_Specification.colorAttachments.size());
            glGenTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());

            for (size_t i = 0; i < m_ColorAttachments.size(); i++)
            {
                glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);

                GLenum internalFormat = TextureFormatToGL(m_Specification.colorAttachments[i]);
                GLenum dataFormat = GL_RGBA;
                GLenum dataType = GL_UNSIGNED_BYTE;

                if (m_Specification.colorAttachments[i] == FramebufferTextureFormat::RGBA16F ||
                    m_Specification.colorAttachments[i] == FramebufferTextureFormat::RGBA32F)
                {
                    dataType = GL_FLOAT;
                }

                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
                             m_Specification.width, m_Specification.height,
                             0, dataFormat, dataType, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i),
                                       GL_TEXTURE_2D, m_ColorAttachments[i], 0);
            }
        }

        // Create depth attachment if needed
        if (m_Specification.hasDepthAttachment)
        {
            glGenTextures(1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
                         m_Specification.width, m_Specification.height,
                         0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_TEXTURE_2D, m_DepthAttachment, 0);
        }

        // Set up draw buffers for MRT
        if (m_ColorAttachments.size() > 1)
        {
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                  GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(static_cast<GLsizei>(m_ColorAttachments.size()), buffers);
        }
        else if (m_ColorAttachments.empty())
        {
            // Only depth attachment
            glDrawBuffer(GL_NONE);
        }

        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            ARV_LOG_ERROR("OpenGLFramebuffer: Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.width, m_Specification.height);
    }

    void OpenGLFramebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
    {
        ARV_LOG_INFO("OpenGLFramebuffer::Resize() - Resizing from {}x{} to {}x{}",
                     m_Specification.width, m_Specification.height, width, height);
        if (width == 0 || height == 0 || width > 8192 || height > 8192)
        {
            ARV_LOG_ERROR("OpenGLFramebuffer::Resize() - Invalid dimensions: {}x{}", width, height);
            return;
        }

        m_Specification.width = width;
        m_Specification.height = height;
        Invalidate();
    }

    uint32_t OpenGLFramebuffer::GetColorAttachmentID(uint32_t index) const
    {
        if (index < m_ColorAttachments.size())
        {
            return m_ColorAttachments[index];
        }
        return 0;
    }

}
