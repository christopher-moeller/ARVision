#include "ExampleTriangleRO.h"
#include "ARVApplication.h"
#include <string>

namespace arv {

    ExampleTriangleRO::ExampleTriangleRO() {
        
        
        ARVApplication* app = ARVApplication::Get();
        
        std::string vertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 aPos;
        
            void main()
            {
                gl_Position = vec4(aPos, 1.0);
            }
        )";

        std::string fragmentShaderSource = R"(
            #version 330 core
        
            out vec4 FragColor;

            void main()
            {
                FragColor = vec4(1.0, 0.5, 0.2, 1.0);
            }
        )";
        

        m_Shader.reset(app->GetRenderer()->CreateShader(vertexShaderSource, fragmentShaderSource));
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
