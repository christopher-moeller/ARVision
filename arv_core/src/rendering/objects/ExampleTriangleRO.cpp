#include "ExampleTriangleRO.h"
#include "../shaders/ExampleShaderSource.h"
#include "../../ARVApplication.h"

namespace arv {

    ExampleTriangleRO::ExampleTriangleRO() {
        
        
        ARVApplication* app = ARVApplication::Get();
            
        m_Shader.reset(app->GetRenderer()->CreateShader(ExampleShaderSource()));
        m_Shader->Compile();
        
        m_VertexArray.reset(app->GetRenderer()->CreateVertexArray());
        
        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };
        
        
        std::shared_ptr<arv::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(app->GetRenderer()->CreateVertexBuffer(vertices, sizeof(vertices)));
        arv::BufferLayout layout = {
                    { arv::ShaderDataType::Float3, "a_Position" },
                    { arv::ShaderDataType::Float4, "a_Color" }
                };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);
        
        uint32_t indices[3] = { 0, 1, 2 };
        std::shared_ptr<arv::IndexBuffer> indexBuffer;
        indexBuffer.reset(app->GetRenderer()->CreateIndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        
        m_VertexArray->Unbind();

    };

    std::shared_ptr<Shader>& ExampleTriangleRO::GetShader() {
        return m_Shader;
    };
    std::shared_ptr<VertexArray>& ExampleTriangleRO::GetVertexArray() {
        return m_VertexArray;
    };

}
