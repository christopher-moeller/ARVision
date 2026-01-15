#pragma once
#include "rendering/RenderingAPI.h"

namespace arv
{
    class MacosOpenGlRenderingAPI : public RenderingAPI
    {
    public:
        MacosOpenGlRenderingAPI();
        ~MacosOpenGlRenderingAPI() override;

        void Init() override;
        void DrawExample() override;

        void Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray) override;

        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;

        VertexBuffer* CreateVertexBuffer(float* vertices, unsigned int size) override;
        IndexBuffer* CreateIndexBuffer(unsigned int* indices, unsigned int size) override;
        VertexArray* CreateVertexArray() override;
        Shader* CreateShader(const ShaderSource& shaderSource) override;
        
    };
}
