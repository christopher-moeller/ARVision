#pragma once

#include <glm/glm.hpp>
#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"
#include "rendering/Shader.h"
#include "rendering/ShaderSource.h"
#include "plattform/PlattformApplicationContext.h"

namespace arv
{
    class RenderingAPI
    {
    public:
        RenderingAPI() = default;
        virtual ~RenderingAPI() = default;

        virtual void Init(PlattformApplicationContext* context) = 0;
        virtual void DrawExample() = 0;

        virtual void Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray) = 0;

        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;

        virtual VertexBuffer* CreateVertexBuffer(float* vertices, unsigned int size) = 0;
        virtual IndexBuffer* CreateIndexBuffer(unsigned int* indices, unsigned int size) = 0;
        virtual VertexArray* CreateVertexArray() = 0;
        virtual Shader* CreateShader(const ShaderSource& shaderSource) = 0;
    };
}
