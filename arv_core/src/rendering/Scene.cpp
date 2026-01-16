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
        //shader->UploadUniformMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
        //shader->UploadUniformMat4("u_Transform", transform);
        
        //shader->UploadUniformFloat4("u_Color", {0.0f, 1.0f, 0.0f, 1.0f});
        
        m_RenderingAPI->Draw(shader, vertexArray);
    }

    void Scene::Render() {
        // TODO: render all commands just here
    }

}
