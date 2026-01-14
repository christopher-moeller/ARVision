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
        void Draw() override;

        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;

        VertexBuffer* CreateVertexBuffer(float* vertices, unsigned int size) override;
        IndexBuffer* CreateIndexBuffer(unsigned int* indices, unsigned int size) override;
        VertexArray* CreateVertexArray() override;
        Shader* CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) override;
        Shader* CreateShader(const ShaderSource& shaderSource) override;

    private:
        unsigned int m_shaderProgram = 0;
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;
    };
}
