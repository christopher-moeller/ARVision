#include "MacosOpenGlRenderingAPI.h"
#include <glad/glad.h>
#include <iostream>

#include "OpenGLBuffer.h"
#include "OpenGLShader.h"
#include "OpenGLVertexArray.h"

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

    void MacosOpenGlRenderingAPI::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void MacosOpenGlRenderingAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    
}
