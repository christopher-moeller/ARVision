#pragma once

#include "camera/StandardCamera.h"
#include "camera/StandardCameraController.h"
#include "rendering/Renderer.h"
#include "rendering/RenderingObject.h"
#include "rendering/Framebuffer.h"
#include "events/EventManager.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace arv { class SelectionCubeRO; }

class SceneDisplaySection {
public:
    SceneDisplaySection(arv::Renderer* renderer, arv::RenderingAPI* renderingAPI,
                        arv::EventManager* eventManager,
                        std::vector<std::unique_ptr<arv::RenderingObject>>* objects,
                        int* selectedObjectIndex);

    ~SceneDisplaySection();

    void Init(int width, int height, const glm::vec4& backgroundColor);
    void Shutdown();
    void Update(float deltaTime);
    void RenderSceneToFramebuffer();
    void RenderImGuiPanel();

    const glm::vec2& GetViewportSize() const { return m_ViewportSize; }
    void SetBackgroundColor(const glm::vec4& color) { m_BackgroundColor = color; }

private:
    // Non-owning references
    arv::Renderer* m_Renderer;
    arv::RenderingAPI* m_RenderingAPI;
    arv::EventManager* m_EventManager;
    std::vector<std::unique_ptr<arv::RenderingObject>>* m_Objects;
    int* m_SelectedObjectIndex;

    // Owned state
    std::unique_ptr<arv::StandardCamera> m_Camera;
    std::unique_ptr<arv::CameraController<arv::StandardCamera>> m_CameraController;
    std::shared_ptr<arv::Framebuffer> m_SceneFramebuffer;
    std::unique_ptr<arv::SelectionCubeRO> m_SelectionCube;
    glm::vec2 m_ViewportSize{0.0f, 0.0f};
    glm::vec4 m_BackgroundColor{0.2f, 0.3f, 0.3f, 1.0f};
};
