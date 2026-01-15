#pragma once
#include "rendering/RenderingAPI.h"

#ifdef __OBJC__
@protocol MTLDevice;
@protocol MTLCommandQueue;
@class CAMetalLayer;
#else
typedef void MTLDevice;
typedef void MTLCommandQueue;
typedef void CAMetalLayer;
#endif

namespace arv
{
    class MacosMetalRenderingAPI : public RenderingAPI
    {
    public:
        MacosMetalRenderingAPI();
        ~MacosMetalRenderingAPI() override;

        void Init() override;
        void DrawExample() override;

        void Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray) override;

        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;

        VertexBuffer* CreateVertexBuffer(float* vertices, unsigned int size) override;
        IndexBuffer* CreateIndexBuffer(unsigned int* indices, unsigned int size) override;
        VertexArray* CreateVertexArray() override;
        Shader* CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) override;

        void SetMetalLayer(CAMetalLayer* layer);

#ifdef __OBJC__
        id<MTLDevice> GetDevice() const { return m_device; }
        id<MTLCommandQueue> GetCommandQueue() const { return m_commandQueue; }
#endif

    private:
#ifdef __OBJC__
        id<MTLDevice> m_device = nullptr;
        id<MTLCommandQueue> m_commandQueue = nullptr;
#else
        void* m_device = nullptr;
        void* m_commandQueue = nullptr;
#endif
        CAMetalLayer* m_metalLayer = nullptr;
        glm::vec4 m_clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    };
}
