#include "OpenGLBuffer.h"
#include "ARVBase.h"
#include <glad/glad.h>

namespace arv {

    /////////////////////////////////////////////////////////////////////////////
    // VertexBuffer /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        ARV_LOG_INFO("OpenGLVertexBuffer::OpenGLVertexBuffer() - Created vertex buffer ID {} with {} bytes", m_RendererID, size);
    }
    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        ARV_LOG_INFO("OpenGLVertexBuffer::~OpenGLVertexBuffer() - Destroying vertex buffer ID {}", m_RendererID);
        glDeleteBuffers(1, &m_RendererID);
    }
    void OpenGLVertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }
    void OpenGLVertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    /////////////////////////////////////////////////////////////////////////////
    // IndexBuffer //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
        : m_Count(count)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
        ARV_LOG_INFO("OpenGLIndexBuffer::OpenGLIndexBuffer() - Created index buffer ID {} with {} indices", m_RendererID, count);
    }
    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        ARV_LOG_INFO("OpenGLIndexBuffer::~OpenGLIndexBuffer() - Destroying index buffer ID {}", m_RendererID);
        glDeleteBuffers(1, &m_RendererID);
    }
    void OpenGLIndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }
    void OpenGLIndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }


}