#pragma once

#include <memory>
#include "rendering/RenderingAPI.h"
#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"
#include "rendering/Shader.h"
#include "rendering/ShaderSource.h"
#include "RenderingObject.h"
#include "Scene.h"

namespace arv {

    class Renderer {
    public:
        Renderer(RenderingAPI* renderingAPI);
        void DrawObject(RenderingObject& object);


        void Init();
        void OnTargetResize(uint32_t width, uint32_t height);

        std::shared_ptr<VertexBuffer> CreateVertexBuffer(float* vertices, unsigned int size);
        std::shared_ptr<IndexBuffer> CreateIndexBuffer(unsigned int* indices, unsigned int size);
        std::shared_ptr<VertexArray> CreateVertexArray();
        std::shared_ptr<Shader> CreateShader(ShaderSource* shaderSource);

        Scene NewScene(Camera* camera);

    private:
        std::shared_ptr<RenderingAPI> m_RenderingAPI;
    };

}
