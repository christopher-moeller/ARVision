#include "MacosOpenGlRenderingAPI.h"
#include "ARVBase.h"
#include <glad/glad.h>
#include <iostream>

#include "OpenGLBuffer.h"
#include "OpenGLShader.h"
#include "OpenGLVertexArray.h"
#include "OpenGLTexture.h"
#include "OpenGLHDRTexture.h"
#include "OpenGLFramebuffer.h"

namespace arv
{


    MacosOpenGlRenderingAPI::MacosOpenGlRenderingAPI()
    {

    }

    MacosOpenGlRenderingAPI::~MacosOpenGlRenderingAPI()
    {
        if (m_defaultTextureID != 0)
        {
            glDeleteTextures(1, &m_defaultTextureID);
            m_defaultTextureID = 0;
        }
    }

    void MacosOpenGlRenderingAPI::Init(PlatformApplicationContext* context)
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::Init() - Enabling depth testing");
        glEnable(GL_DEPTH_TEST);
        CreateDefaultTexture();
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::Init() - OpenGL Rendering API initialized");
    }

    void MacosOpenGlRenderingAPI::CreateDefaultTexture()
    {
        // Create a 1x1 white texture as fallback for objects without textures
        glGenTextures(1, &m_defaultTextureID);
        glBindTexture(GL_TEXTURE_2D, m_defaultTextureID);

        // White pixel (RGBA: 255, 255, 255, 255)
        uint8_t whitePixel[4] = {255, 255, 255, 255};

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

        glBindTexture(GL_TEXTURE_2D, 0);
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::CreateDefaultTexture() - Created default white texture");
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

        // Clear the default framebuffer at the start of each frame
        // This ensures no leftover content from previous frames
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void MacosOpenGlRenderingAPI::FlushDrawCommands()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (const auto& cmd : m_drawCommands)
        {
            // Bind texture - use provided texture or fall back to default white texture
            bool textureSet = false;
            if (cmd.texture && cmd.texture->IsValid())
            {
                cmd.texture->Bind(0);
                textureSet = true;
            }

            // If no valid texture was set, use the default white texture
            if (!textureSet && m_defaultTextureID != 0)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_defaultTextureID);
            }

            cmd.shader->Use();
            cmd.vertexArray->Bind();
            glDrawElements(GL_TRIANGLES, cmd.vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

            if (textureSet)
            {
                cmd.texture->Unbind();
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        glDisable(GL_BLEND);
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

    std::shared_ptr<Texture2D> MacosOpenGlRenderingAPI::CreateHDRTexture2D(const std::string& path)
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::CreateHDRTexture2D() - Creating HDR texture from path: {}", path);
        return std::make_shared<OpenGLHDRTexture2D>(path);
    }

    std::shared_ptr<Framebuffer> MacosOpenGlRenderingAPI::CreateFramebuffer(const FramebufferSpecification& spec)
    {
        ARV_LOG_INFO("MacosOpenGlRenderingAPI::CreateFramebuffer() - Creating framebuffer {}x{}", spec.width, spec.height);
        return std::make_shared<OpenGLFramebuffer>(spec);
    }

}
