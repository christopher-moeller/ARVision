#pragma once
#include "rendering/RenderingAPI.h"

namespace arv
{
    class MacosOpenGlRenderingAPI : public RenderingAPI
    {
    public:
        MacosOpenGlRenderingAPI();
        ~MacosOpenGlRenderingAPI() override;

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

    };
}
