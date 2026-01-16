#include "Renderer.h"
#include <glm/glm.hpp>

namespace arv {

    Renderer::Renderer(RenderingAPI* renderingAPI)
    {
        m_RenderingAPI.reset(renderingAPI);
    }

    void Renderer::DrawObject(RenderingObject& object)
    {
        m_RenderingAPI->SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
        m_RenderingAPI->Clear();
        m_RenderingAPI->Draw(object.GetShader(), object.GetVertexArray());
    }

    void Renderer::Init()
    {
        //m_RenderingAPI->Init();
    }

    void Renderer::OnTargetResize(uint32_t width, uint32_t height)
    {
        
    }

    VertexBuffer* Renderer::CreateVertexBuffer(float* vertices, unsigned int size)
    {
        return m_RenderingAPI->CreateVertexBuffer(vertices, size);
    }

    IndexBuffer* Renderer::CreateIndexBuffer(unsigned int* indices, unsigned int size)
    {
        return m_RenderingAPI->CreateIndexBuffer(indices, size);
    }

    VertexArray* Renderer::CreateVertexArray()
    {
        return m_RenderingAPI->CreateVertexArray();
    }

    Shader* Renderer::CreateShader(ShaderSource* shaderSource)
    {
        return m_RenderingAPI->CreateShader(shaderSource);
    }

    Scene Renderer::NewScene(Camera* camera) {
        return Scene(m_RenderingAPI, camera);
    }

}
