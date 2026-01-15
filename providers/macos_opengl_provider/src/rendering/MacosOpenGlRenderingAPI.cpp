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

    void MacosOpenGlRenderingAPI::Init()
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

    VertexBuffer* MacosOpenGlRenderingAPI::CreateVertexBuffer(float* vertices, unsigned int size)
    {
        return new OpenGLVertexBuffer(vertices, size);
    }

    IndexBuffer* MacosOpenGlRenderingAPI::CreateIndexBuffer(unsigned int* indices, unsigned int size)
    {
        return new OpenGLIndexBuffer(indices, size);
    }

    VertexArray* MacosOpenGlRenderingAPI::CreateVertexArray()
    {
        return new OpenGLVertexArray();
    }

    Shader* MacosOpenGlRenderingAPI::CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
    {
       return new OpenGLShader(vertexShaderSource, fragmentShaderSource);
    }
    
}
