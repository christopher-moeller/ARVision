#include "MetalVertexArray.h"

namespace arv {

    MetalVertexArray::MetalVertexArray()
    {
        // Metal doesn't have VAOs - this class serves as a container
        // for vertex and index buffers
    }

    MetalVertexArray::~MetalVertexArray()
    {
    }

    void MetalVertexArray::Bind() const
    {
        // Metal doesn't have a global bind state like OpenGL
        // Buffer binding happens during command encoding
    }

    void MetalVertexArray::Unbind() const
    {
        // No-op for Metal
    }

    void MetalVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
    {
        // Metal doesn't require setting up vertex attribute pointers here
        // The vertex descriptor is defined in the pipeline state
        // We just store the buffer reference
        m_VertexBuffers.push_back(vertexBuffer);
    }

    void MetalVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
    {
        m_IndexBuffer = indexBuffer;
    }

}
