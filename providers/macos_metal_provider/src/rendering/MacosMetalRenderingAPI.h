#pragma once
#include "rendering/RenderingAPI.h"

#ifdef __OBJC__
@protocol MTLDevice;
@protocol MTLCommandQueue;
@protocol MTLCommandBuffer;
@protocol MTLRenderCommandEncoder;
@protocol CAMetalDrawable;
@class CAMetalLayer;
@class MTLRenderPassDescriptor;
#else
typedef void MTLDevice;
typedef void MTLCommandQueue;
typedef void MTLCommandBuffer;
typedef void MTLRenderCommandEncoder;
typedef void CAMetalDrawable;
typedef void CAMetalLayer;
typedef void MTLRenderPassDescriptor;
#endif

namespace arv
{
    class MacosMetalRenderingAPI : public RenderingAPI
    {
    public:
        MacosMetalRenderingAPI();
        ~MacosMetalRenderingAPI() override;

        void Init(PlattformApplicationContext* context) override;
        void DrawExample() override;

        void Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray) override;
        void Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture) override;

        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;

        void BeginFrame() override;
        void EndFrame() override;

        std::shared_ptr<VertexBuffer> CreateVertexBuffer(float* vertices, unsigned int size) override;
        std::shared_ptr<IndexBuffer> CreateIndexBuffer(unsigned int* indices, unsigned int size) override;
        std::shared_ptr<VertexArray> CreateVertexArray() override;
        std::shared_ptr<Shader> CreateShader(ShaderSource* shaderSource) override;
        std::shared_ptr<Texture2D> CreateTexture2D(const std::string& path) override;

        void SetMetalLayer(CAMetalLayer* layer);

#ifdef __OBJC__
        id<MTLDevice> GetDevice() const { return m_device; }
        id<MTLCommandQueue> GetCommandQueue() const { return m_commandQueue; }
#endif

    private:
        void DrawInternal(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture);

#ifdef __OBJC__
        id<MTLDevice> m_device = nullptr;
        id<MTLCommandQueue> m_commandQueue = nullptr;
        id<MTLCommandBuffer> m_currentCommandBuffer = nullptr;
        id<MTLRenderCommandEncoder> m_currentRenderEncoder = nullptr;
        id<CAMetalDrawable> m_currentDrawable = nullptr;
#else
        void* m_device = nullptr;
        void* m_commandQueue = nullptr;
        void* m_currentCommandBuffer = nullptr;
        void* m_currentRenderEncoder = nullptr;
        void* m_currentDrawable = nullptr;
#endif
        CAMetalLayer* m_metalLayer = nullptr;
        glm::vec4 m_clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        bool m_frameInProgress = false;
    };
}
