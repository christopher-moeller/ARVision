#include "MacosOpenGlRenderingAPI.h"
#include "ARVBase.h"
#include <glad/glad.h>
#include <iostream>

#include "OpenGLBuffer.h"
#include "OpenGLShader.h"
#include "OpenGLVertexArray.h"
#include "OpenGLTexture.h"
#include "OpenGLFramebuffer.h"

namespace arv
{


    MacosOpenGlRenderingAPI::MacosOpenGlRenderingAPI()
    {

    }

    MacosOpenGlRenderingAPI::~MacosOpenGlRenderingAPI()
    {

    }

    void MacosOpenGlRenderingAPI::Init(PlatformApplicationContext* context)
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::Init() - Enabling depth testing");
        glEnable(GL_DEPTH_TEST);
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::Init() - OpenGL Rendering API initialized");
    }

    void MacosOpenGlRenderingAPI::DrawExample()
    {

    }

    void MacosOpenGlRenderingAPI::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
    {
        m_drawCommands.push_back({shader, vertexArray, nullptr});
    }

    void MacosOpenGlRenderingAPI::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture)
    {
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

    void MacosOpenGlRenderingAPI::FlushDrawCommands()
    {
        for (const auto& cmd : m_drawCommands)
        {
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

        m_drawCommands.clear();
    }

    void MacosOpenGlRenderingAPI::EndFrame()
    {
        if (!m_frameInProgress)
        {
            return;
        }

        // Execute any remaining draw commands
        FlushDrawCommands();

        m_frameInProgress = false;
    }

    std::shared_ptr<VertexBuffer> MacosOpenGlRenderingAPI::CreateVertexBuffer(float* vertices, unsigned int size)
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::CreateVertexBuffer() - Creating vertex buffer with {} bytes", size);
        return std::make_shared<OpenGLVertexBuffer>(vertices, size);
    }

    std::shared_ptr<IndexBuffer> MacosOpenGlRenderingAPI::CreateIndexBuffer(unsigned int* indices, unsigned int size)
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::CreateIndexBuffer() - Creating index buffer with {} indices", size);
        return std::make_shared<OpenGLIndexBuffer>(indices, size);
    }

    std::shared_ptr<VertexArray> MacosOpenGlRenderingAPI::CreateVertexArray()
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::CreateVertexArray() - Creating vertex array");
        return std::make_shared<OpenGLVertexArray>();
    }

    std::shared_ptr<Shader> MacosOpenGlRenderingAPI::CreateShader(ShaderSource* shaderSource)
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::CreateShader() - Creating shader from source");
        return std::make_shared<OpenGLShader>(shaderSource);
    }

    std::shared_ptr<Texture2D> MacosOpenGlRenderingAPI::CreateTexture2D(const std::string& path)
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::CreateTexture2D() - Creating texture from path: {}", path);
        return std::make_shared<OpenGLTexture2D>(path);
    }

    std::shared_ptr<Framebuffer> MacosOpenGlRenderingAPI::CreateFramebuffer(const FramebufferSpecification& spec)
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::CreateFramebuffer() - Creating framebuffer {}x{}", spec.width, spec.height);
        return std::make_shared<OpenGLFramebuffer>(spec);
    }

}
