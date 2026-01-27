#pragma once

#include "Layer.h"
#include "camera/StandardCamera.h"
#include "camera/StandardCameraController.h"
#include "rendering/Renderer.h"
#include "rendering/RenderingObject.h"
#include "rendering/Framebuffer.h"
#include "platform/Canvas.h"
#include "events/EventManager.h"
#include <memory>
#include <vector>
#include <chrono>
#include <glm/glm.hpp>

namespace arv { class SelectionCubeRO; }

class MainLayer : public arv::Layer {
public:
    MainLayer(arv::Renderer* renderer, arv::Canvas* canvas,
              arv::RenderingAPI* renderingAPI, arv::EventManager* eventManager,
              int width, int height);
    ~MainLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;

private:
    void InitImGui();
    void ShutdownImGui();
    void BeginImGui();
    void EndImGui();
    void RenderSceneToFramebuffer();
    void RenderImGuiUI();

    // Renderer and API references
    arv::Renderer* m_Renderer;
    arv::Canvas* m_Canvas;
    arv::RenderingAPI* m_RenderingAPI;
    arv::EventManager* m_EventManager;
    int m_WindowWidth;
    int m_WindowHeight;

    // Scene components
    std::unique_ptr<arv::StandardCamera> m_Camera;
    std::unique_ptr<arv::CameraController<arv::StandardCamera>> m_CameraController;
    std::vector<std::unique_ptr<arv::RenderingObject>> m_Objects;
    glm::vec4 m_BackgroundColor{0.2f, 0.3f, 0.3f, 1.0f};

    // Framebuffer for scene rendering
    std::shared_ptr<arv::Framebuffer> m_SceneFramebuffer;
    glm::vec2 m_ViewportSize{0.0f, 0.0f};

    // ImGui state
    bool m_ImGuiInitialized = false;
    int m_SelectedObjectIndex = -1;  // -1 means no selection

    // Selection overlay
    std::unique_ptr<arv::SelectionCubeRO> m_SelectionCube;

    // Animation
    std::chrono::high_resolution_clock::time_point m_StartTime;
};
