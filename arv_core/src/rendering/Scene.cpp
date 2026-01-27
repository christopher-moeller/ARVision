#include "Scene.h"
#include "ARVBase.h"

#include <glm/gtc/matrix_transform.hpp>

namespace arv {

    Scene::Scene(RenderingAPI* renderingApi, Camera* camera)
        : m_RenderingAPI(renderingApi), m_Camera(camera)
    {
        
    }

    void Scene::ClearColor(const glm::vec4 &color) {
        m_RenderingAPI->SetClearColor(color);
        m_RenderingAPI->Clear();
        // Note: BeginFrame is now called from main loop to allow multiple layers to render
    }

    void Scene::Submit(RenderingObject& object) {
        glm::mat4 projection = m_Camera->GetProjectionMatrix();
        glm::mat4 view = m_Camera->GetViewMatrix();
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), object.GetPosition());
        const glm::vec3& rot = object.GetRotation();
        transform = glm::rotate(transform, glm::radians(rot.y), glm::vec3(0, 1, 0));
        transform = glm::rotate(transform, glm::radians(rot.x), glm::vec3(1, 0, 0));
        transform = glm::rotate(transform, glm::radians(rot.z), glm::vec3(0, 0, 1));
        transform = glm::scale(transform, object.GetScale());

        glm::mat4 mvp = projection * view * transform;

        object.GetShader()->UploadUniformMat4("u_mvp", mvp);

        auto texture = object.GetTexture();
        if (texture) {
            m_RenderingAPI->Draw(object.GetShader(), object.GetVertexArray(), texture);
        } else {
            m_RenderingAPI->Draw(object.GetShader(), object.GetVertexArray());
        }
    }

    void Scene::Render() {
        // Flush any pending draw commands so they execute before overlays (like ImGui) render
        m_RenderingAPI->FlushDrawCommands();
    }

}
