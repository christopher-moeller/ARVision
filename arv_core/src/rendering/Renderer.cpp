#include "Renderer.h"
#include "ARVBase.h"
#include <glm/glm.hpp>

namespace arv {

    Renderer::Renderer(RenderingAPI* renderingAPI)
        : m_RenderingAPI(renderingAPI)
    {
        ARV_LOG_INFO("Renderer::Renderer() - Renderer created with RenderingAPI");
    }

    void Renderer::DrawObject(RenderingObject& object)
    {
        m_RenderingAPI->SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
        m_RenderingAPI->Clear();
        m_RenderingAPI->Draw(object.GetShader(), object.GetVertexArray());
    }

    void Renderer::Init()
    {
        ARV_LOG_INFO("Renderer::Init() - Initializing renderer");
    }

    void Renderer::OnTargetResize(uint32_t width, uint32_t height)
    {
        ARV_LOG_INFO("Renderer::OnTargetResize() - Target resized to {}x{}", width, height);
    }

    std::shared_ptr<VertexBuffer> Renderer::CreateVertexBuffer(float* vertices, unsigned int size)
    {
        ARV_LOG_INFO("Renderer::CreateVertexBuffer() - Creating vertex buffer with size {} bytes", size);
        return m_RenderingAPI->CreateVertexBuffer(vertices, size);
    }

    std::shared_ptr<IndexBuffer> Renderer::CreateIndexBuffer(unsigned int* indices, unsigned int size)
    {
        ARV_LOG_INFO("Renderer::CreateIndexBuffer() - Creating index buffer with {} indices", size);
        return m_RenderingAPI->CreateIndexBuffer(indices, size);
    }

    std::shared_ptr<VertexArray> Renderer::CreateVertexArray()
    {
        ARV_LOG_INFO("Renderer::CreateVertexArray() - Creating vertex array");
        return m_RenderingAPI->CreateVertexArray();
    }

    std::shared_ptr<Shader> Renderer::CreateShader(ShaderSource* shaderSource)
    {
        ARV_LOG_INFO("Renderer::CreateShader() - Creating shader from source");
        return m_RenderingAPI->CreateShader(shaderSource);
    }

    std::shared_ptr<Texture2D> Renderer::CreateTexture2D(const std::string& path)
    {
        ARV_LOG_INFO("Renderer::CreateTexture2D() - Creating texture from path: {}", path);
        return m_RenderingAPI->CreateTexture2D(path);
    }

    Scene Renderer::NewScene(Camera* camera) {
        return Scene(m_RenderingAPI, camera);
    }

}
