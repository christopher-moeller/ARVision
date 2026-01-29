#include "RenderingObject.h"
#include "Renderer.h"

namespace arv {

    RenderingObject::RenderingResources RenderingObject::SetupRendering(
        Renderer* renderer,
        const std::string& shaderSourceCode,
        const float* vertices, size_t verticesSizeBytes,
        const uint32_t* indices, size_t indexCount,
        const BufferLayout& layout)
    {
        RenderingResources resources;

        resources.shaderSource = std::make_unique<CoreShaderSource>(shaderSourceCode);
        resources.shader = renderer->CreateShader(resources.shaderSource.get());
        resources.shader->Compile();

        resources.vertexArray = renderer->CreateVertexArray();

        auto vertexBuffer = renderer->CreateVertexBuffer(
            const_cast<float*>(vertices), static_cast<unsigned int>(verticesSizeBytes));
        vertexBuffer->SetLayout(layout);
        resources.vertexArray->AddVertexBuffer(vertexBuffer);

        auto indexBuffer = renderer->CreateIndexBuffer(
            const_cast<uint32_t*>(indices), static_cast<unsigned int>(indexCount));
        resources.vertexArray->SetIndexBuffer(indexBuffer);

        resources.vertexArray->Unbind();

        return resources;
    }

}
