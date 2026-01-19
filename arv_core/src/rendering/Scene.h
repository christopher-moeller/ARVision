#pragma once

#include "rendering/RenderingAPI.h"
#include "../camera/Camera.h"
#include <memory>
#include "RenderingObject.h"

namespace arv {

    class Scene {
    public:
        Scene(RenderingAPI* renderingApi, Camera* camera);

        void Submit(RenderingObject& object);
        void Submit(RenderingObject& object, const std::shared_ptr<Texture2D>& texture);
        void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);
        void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture2D>& texture);
        void ClearColor(const glm::vec4& color);
        void Render();

    private:
        RenderingAPI* m_RenderingAPI;  // Non-owning pointer
        Camera* m_Camera;
    };

}
