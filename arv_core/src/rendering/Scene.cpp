#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>

namespace arv {

    Scene::Scene(std::shared_ptr<RenderingAPI>& renderingApi, Camera* camera) : m_RenderingAPI(renderingApi), m_Camera(camera) {
        
    }

    void Scene::ClearColor(const glm::vec4 &color) {
        m_RenderingAPI->SetClearColor(color);
        m_RenderingAPI->Clear();
    }

    void Scene::Submit(RenderingObject& object) {
        Submit(object.GetShader(), object.GetVertexArray());
    }

    void Scene::Submit(const std::shared_ptr<Shader> &shader, const std::shared_ptr<VertexArray> &vertexArray) {
        glm::mat4 projection = m_Camera->GetProjectionMatrix();
        glm::mat4 view = m_Camera->GetViewMatrix();
        glm::mat4 transform(1.0f);

        glm::mat4 mvp = projection * view * transform;

        shader->UploadUniformMat4("u_mvp", mvp);

        m_RenderingAPI->Draw(shader, vertexArray);
    }

    void Scene::Submit(const std::shared_ptr<Shader> &shader, const std::shared_ptr<VertexArray> &vertexArray, const std::shared_ptr<Texture2D> &texture) {
        glm::mat4 projection = m_Camera->GetProjectionMatrix();
        glm::mat4 view = m_Camera->GetViewMatrix();
        glm::mat4 transform(1.0f);

        glm::mat4 mvp = projection * view * transform;

        shader->UploadUniformMat4("u_mvp", mvp);

        m_RenderingAPI->Draw(shader, vertexArray, texture);
    }

    void Scene::Render() {
        // TODO: render all commands just here
    }

}
