#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"
#include "rendering/Shader.h"
#include "rendering/ShaderSource.h"
#include "rendering/Texture.h"
#include "rendering/Framebuffer.h"
#include "platform/PlatformApplicationContext.h"

namespace arv
{
    enum class RenderingBackend
    {
        None = 0,
        OpenGL,
        Metal
    };

    class RenderingAPI
    {
    public:
        RenderingAPI() = default;
        virtual ~RenderingAPI() = default;

        virtual RenderingBackend GetBackendType() const = 0;

        virtual void Init(PlatformApplicationContext* context) = 0;
        virtual void DrawExample() = 0;

        virtual void Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray) = 0;
        virtual void Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture) = 0;

        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer(float* vertices, unsigned int size) = 0;
        virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer(unsigned int* indices, unsigned int size) = 0;
        virtual std::shared_ptr<VertexArray> CreateVertexArray() = 0;
        virtual std::shared_ptr<Shader> CreateShader(ShaderSource* shaderSource) = 0;
        virtual std::shared_ptr<Texture2D> CreateTexture2D(const std::string& path) = 0;
        virtual std::shared_ptr<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec) = 0;
    };
}
