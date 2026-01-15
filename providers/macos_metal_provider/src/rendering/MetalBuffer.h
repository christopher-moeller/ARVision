#pragma once

#include "rendering/Buffer.h"

#ifdef __OBJC__
@protocol MTLBuffer;
@protocol MTLDevice;
#else
typedef void MTLBuffer;
typedef void MTLDevice;
#endif

namespace arv {

    class MetalVertexBuffer : public VertexBuffer
    {
    public:
#ifdef __OBJC__
        MetalVertexBuffer(id<MTLDevice> device, float* vertices, unsigned int size);
#else
        MetalVertexBuffer(void* device, float* vertices, unsigned int size);
#endif
        virtual ~MetalVertexBuffer();
        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual const BufferLayout& GetLayout() const override { return m_Layout; }
        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

#ifdef __OBJC__
        id<MTLBuffer> GetMetalBuffer() const { return m_buffer; }
#else
        void* GetMetalBuffer() const { return m_buffer; }
#endif

    private:
#ifdef __OBJC__
        id<MTLBuffer> m_buffer = nullptr;
#else
        void* m_buffer = nullptr;
#endif
        BufferLayout m_Layout;
    };

    class MetalIndexBuffer : public IndexBuffer
    {
    public:
#ifdef __OBJC__
        MetalIndexBuffer(id<MTLDevice> device, unsigned int* indices, unsigned int count);
#else
        MetalIndexBuffer(void* device, unsigned int* indices, unsigned int count);
#endif
        virtual ~MetalIndexBuffer();
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual unsigned int GetCount() const override { return m_Count; }

#ifdef __OBJC__
        id<MTLBuffer> GetMetalBuffer() const { return m_buffer; }
#else
        void* GetMetalBuffer() const { return m_buffer; }
#endif

    private:
#ifdef __OBJC__
        id<MTLBuffer> m_buffer = nullptr;
#else
        void* m_buffer = nullptr;
#endif
        unsigned int m_Count;
    };

}
