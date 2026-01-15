#include "MetalBuffer.h"

#import <Metal/Metal.h>

namespace arv {

    /////////////////////////////////////////////////////////////////////////////
    // MetalVertexBuffer ////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    MetalVertexBuffer::MetalVertexBuffer(id<MTLDevice> device, float* vertices, unsigned int size)
    {
        m_buffer = [device newBufferWithBytes:vertices
                                       length:size
                                      options:MTLResourceStorageModeShared];
    }

    MetalVertexBuffer::~MetalVertexBuffer()
    {
        m_buffer = nil;
    }

    void MetalVertexBuffer::Bind() const
    {
        // Metal doesn't have a global bind concept like OpenGL
        // Binding happens during command encoding in the rendering API
    }

    void MetalVertexBuffer::Unbind() const
    {
        // No-op for Metal
    }

    /////////////////////////////////////////////////////////////////////////////
    // MetalIndexBuffer /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    MetalIndexBuffer::MetalIndexBuffer(id<MTLDevice> device, unsigned int* indices, unsigned int count)
        : m_Count(count)
    {
        m_buffer = [device newBufferWithBytes:indices
                                       length:count * sizeof(unsigned int)
                                      options:MTLResourceStorageModeShared];
    }

    MetalIndexBuffer::~MetalIndexBuffer()
    {
        m_buffer = nil;
    }

    void MetalIndexBuffer::Bind() const
    {
        // Metal doesn't have a global bind concept like OpenGL
        // Binding happens during command encoding in the rendering API
    }

    void MetalIndexBuffer::Unbind() const
    {
        // No-op for Metal
    }

}
