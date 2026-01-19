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
        // Queue the draw command for later execution
        m_drawCommands.push_back({shader, vertexArray, nullptr});
    }

    void MacosOpenGlRenderingAPI::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture)
    {
        // Queue the draw command for later execution
        m_drawCommands.push_back({shader, vertexArray, texture});
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
        m_drawCommands.clear();
        m_frameInProgress = true;
    }

    void MacosOpenGlRenderingAPI::EndFrame()
    {
        if (!m_frameInProgress)
        {
            return;
        }

        // Execute all queued draw commands
        for (const auto& cmd : m_drawCommands)
        {
            // Enable blending for alpha transparency if texture is present
            if (cmd.texture)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                cmd.texture->Bind(0);
            }

            cmd.shader->Use();
            cmd.vertexArray->Bind();
            glDrawElements(GL_TRIANGLES, cmd.vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

            if (cmd.texture)
            {
                cmd.texture->Unbind();
            }
        }

        // Clear the command queue for the next frame
        m_drawCommands.clear();
        m_frameInProgress = false;
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
