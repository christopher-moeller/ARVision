#pragma once

#include "camera/StandardCamera.h"
#include "camera/StandardCameraController.h"
#include "rendering/Renderer.h"
#include "rendering/RenderingObject.h"
#include "rendering/Framebuffer.h"
#include "events/EventManager.h"
#include "../EditorState.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <functional>

namespace arv { class SelectionCubeRO; class SkyboxRO; }

class SceneDisplaySection {
public:
    SceneDisplaySection(arv::Renderer* renderer, arv::RenderingAPI* renderingAPI,
                        arv::EventManager* eventManager, EditorState* state);

    ~SceneDisplaySection();

    void Init(int width, int height);
    void Shutdown();
    void Update(float deltaTime);
    void RenderSceneToFramebuffer();
    void RenderImGuiPanel();

    const glm::vec2& GetViewportSize() const { return m_ViewportSize; }
    void LoadSkyboxTexture(const std::string& path);
    bool TakeScreenshot(const std::string& filepath);
    std::vector<unsigned char> CaptureFramebuffer(uint32_t& outWidth, uint32_t& outHeight);
    glm::mat4 GetViewProjectionMatrix() const;

private:
    void RenderSkybox();
    void SubmitScene();
    void RenderSelectionCube();
    // Non-owning references
    arv::Renderer* m_Renderer;
    arv::RenderingAPI* m_RenderingAPI;
    arv::EventManager* m_EventManager;
    EditorState* m_State;

    // Owned state
    std::unique_ptr<arv::StandardCamera> m_Camera;
    std::unique_ptr<arv::CameraController<arv::StandardCamera>> m_CameraController;
    std::shared_ptr<arv::Framebuffer> m_SceneFramebuffer;
    std::unique_ptr<arv::SelectionCubeRO> m_SelectionCube;
    std::unique_ptr<arv::SkyboxRO> m_Skybox;
    std::shared_ptr<arv::Texture2D> m_SkyboxTexture;
    glm::vec2 m_ViewportSize{0.0f, 0.0f};

    // Cached VP matrix from the last render (ensures recording matches rendered frame)
    glm::mat4 m_LastRenderedVP{1.0f};
};
