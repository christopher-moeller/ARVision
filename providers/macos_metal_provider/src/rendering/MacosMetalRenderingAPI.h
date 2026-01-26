#pragma once
#include "rendering/RenderingAPI.h"

#ifdef __OBJC__
@protocol MTLDevice;
@protocol MTLCommandQueue;
@protocol MTLCommandBuffer;
@protocol MTLRenderCommandEncoder;
@protocol MTLDepthStencilState;
@protocol MTLTexture;
@protocol CAMetalDrawable;
@class CAMetalLayer;
@class MTLRenderPassDescriptor;
#else
typedef void MTLDevice;
typedef void MTLCommandQueue;
typedef void MTLCommandBuffer;
typedef void MTLRenderCommandEncoder;
typedef void MTLDepthStencilState;
typedef void MTLTexture;
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

        RenderingBackend GetBackendType() const override { return RenderingBackend::Metal; }

        void Init(PlatformApplicationContext* context) override;
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
        std::shared_ptr<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec) override;

        void SetMetalLayer(CAMetalLayer* layer);
        void BindFramebuffer(const std::shared_ptr<Framebuffer>& framebuffer);
        void UnbindFramebuffer();

#ifdef __OBJC__
        id<MTLDevice> GetDevice() const { return m_device; }
        id<MTLCommandQueue> GetCommandQueue() const { return m_commandQueue; }
        id<MTLCommandBuffer> GetCurrentCommandBuffer() const { return m_currentCommandBuffer; }
        id<CAMetalDrawable> GetCurrentDrawable() const { return m_currentDrawable; }

        // ImGui integration methods
        // Ends the current render encoder but keeps command buffer active for ImGui
        void EndRenderPass();
        // Creates a render pass descriptor for ImGui (loads existing content)
        MTLRenderPassDescriptor* CreateImGuiRenderPassDescriptor();
        // Creates a new render encoder for ImGui
        id<MTLRenderCommandEncoder> CreateImGuiRenderEncoder(MTLRenderPassDescriptor* descriptor);
        // Ends ImGui render pass and prepares for EndFrame
        void EndImGuiRenderPass(id<MTLRenderCommandEncoder> encoder);

        // Framebuffer render pass methods
        // Starts a render pass targeting the specified framebuffer
        void BeginFramebufferPass(const std::shared_ptr<Framebuffer>& framebuffer, const glm::vec4& clearColor);
        // Ends the framebuffer render pass
        void EndFramebufferPass();
        // Get depth stencil state for external use
        id<MTLDepthStencilState> GetDepthStencilState() const { return m_depthStencilState; }
#endif

    private:
        void DrawInternal(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture);
        void CreateDepthTextureIfNeeded(size_t width, size_t height);

#ifdef __OBJC__
        id<MTLDevice> m_device = nullptr;
        id<MTLCommandQueue> m_commandQueue = nullptr;
        id<MTLCommandBuffer> m_currentCommandBuffer = nullptr;
        id<MTLRenderCommandEncoder> m_currentRenderEncoder = nullptr;
        id<CAMetalDrawable> m_currentDrawable = nullptr;
        id<MTLDepthStencilState> m_depthStencilState = nullptr;
        id<MTLTexture> m_depthTexture = nullptr;
#else
        void* m_device = nullptr;
        void* m_commandQueue = nullptr;
        void* m_currentCommandBuffer = nullptr;
        void* m_currentRenderEncoder = nullptr;
        void* m_currentDrawable = nullptr;
        void* m_depthStencilState = nullptr;
        void* m_depthTexture = nullptr;
#endif
        CAMetalLayer* m_metalLayer = nullptr;
        glm::vec4 m_clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        bool m_frameInProgress = false;
        std::shared_ptr<Framebuffer> m_boundFramebuffer = nullptr;
    };
}
