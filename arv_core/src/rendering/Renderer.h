#pragma once

#include <memory>
#include "rendering/RenderingAPI.h"
#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"
#include "rendering/Shader.h"
#include "rendering/shaders/ShaderSource.h"

namespace arv {

    class Renderer {
    public:
        Renderer(RenderingAPI* renderingAPI);
        void Draw(); // Todo only tmp


        void Init();
        void OnTargetResize(uint32_t width, uint32_t height);
        
        VertexBuffer* CreateVertexBuffer(float* vertices, unsigned int size);
        IndexBuffer* CreateIndexBuffer(unsigned int* indices, unsigned int size);
        VertexArray* CreateVertexArray();
        Shader* CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
        Shader* CreateShader(const ShaderSource& shaderSource);

    private:
        std::shared_ptr<RenderingAPI> m_RenderingAPI;
    };

}
