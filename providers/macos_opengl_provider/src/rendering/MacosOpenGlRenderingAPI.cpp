#include "MacosOpenGlRenderingAPI.h"
#include <glad/glad.h>
#include <iostream>

#include "OpenGLBuffer.h"
#include "OpenGLShader.h"
#include "OpenGLVertexArray.h"
#include "OpenGLTexture.h"

namespace arv
{
    

    MacosOpenGlRenderingAPI::MacosOpenGlRenderingAPI()
    {
    }

    MacosOpenGlRenderingAPI::~MacosOpenGlRenderingAPI()
    {
        
    }

    void MacosOpenGlRenderingAPI::Init(PlattformApplicationContext* context)
    {
        
    }

    void MacosOpenGlRenderingAPI::DrawExample()
    {
        
    }

    void MacosOpenGlRenderingAPI::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
    {
        shader->Use();
        vertexArray->Bind();

        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void MacosOpenGlRenderingAPI::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture)
    {
        // Enable blending for alpha transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader->Use();

        // Bind texture to slot 0
        if (texture)
        {
            texture->Bind(0);
        }

        vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

        if (texture)
        {
            texture->Unbind();
        }
    }

    void MacosOpenGlRenderingAPI::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void MacosOpenGlRenderingAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void MacosOpenGlRenderingAPI::BeginFrame()
    {
        // OpenGL doesn't need explicit frame management
    }

    void MacosOpenGlRenderingAPI::EndFrame()
    {
        // OpenGL doesn't need explicit frame management
    }

    std::shared_ptr<VertexBuffer> MacosOpenGlRenderingAPI::CreateVertexBuffer(float* vertices, unsigned int size)
    {
        return std::make_shared<OpenGLVertexBuffer>(vertices, size);
    }

    std::shared_ptr<IndexBuffer> MacosOpenGlRenderingAPI::CreateIndexBuffer(unsigned int* indices, unsigned int size)
    {
        return std::make_shared<OpenGLIndexBuffer>(indices, size);
    }

    std::shared_ptr<VertexArray> MacosOpenGlRenderingAPI::CreateVertexArray()
    {
        return std::make_shared<OpenGLVertexArray>();
    }

    std::shared_ptr<Shader> MacosOpenGlRenderingAPI::CreateShader(ShaderSource* shaderSource)
    {
       return std::make_shared<OpenGLShader>(shaderSource);
    }

    std::shared_ptr<Texture2D> MacosOpenGlRenderingAPI::CreateTexture2D(const std::string& path)
    {
        return std::make_shared<OpenGLTexture2D>(path);
    }

}
