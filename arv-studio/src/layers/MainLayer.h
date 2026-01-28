#pragma once

#include "Layer.h"
#include "rendering/Renderer.h"
#include "rendering/RenderingObject.h"
#include "rendering/RenderingAPI.h"
#include "platform/Canvas.h"
#include "events/EventManager.h"
#include "../sections/SceneDisplaySection.h"
#include "../sections/ControlSection.h"
#include <memory>
#include <vector>
#include <chrono>
#include <string>
#include <glm/glm.hpp>

struct BackgroundSettings {
    enum class Mode { Color, Skybox };
    Mode mode = Mode::Color;
    glm::vec4 color{0.2f, 0.3f, 0.3f, 1.0f};
    std::string skyboxPath;
};

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

    int GetMaxFPS() const { return m_MaxFPS; }

private:
    void InitImGui();
    void ShutdownImGui();
    void BeginImGui();
    void EndImGui();
    void LoadScene(const std::string& path);
    void SaveScene();

    // Core references
    arv::Renderer* m_Renderer;
    arv::Canvas* m_Canvas;
    arv::RenderingAPI* m_RenderingAPI;
    arv::EventManager* m_EventManager;
    int m_WindowWidth;
    int m_WindowHeight;

    // Shared scene data
    std::vector<std::unique_ptr<arv::RenderingObject>> m_Objects;
    int m_SelectedObjectIndex = -1;
    std::string m_CurrentScenePath;
    BackgroundSettings m_BackgroundSettings;
    float m_DeltaTime = 0.0f;
    int m_MaxFPS = 0; // 0 = unlimited

    // Sections
    std::unique_ptr<SceneDisplaySection> m_SceneDisplay;
    std::unique_ptr<ControlSection> m_ControlSection;

    // ImGui state
    bool m_ImGuiInitialized = false;

    // Animation
    std::chrono::high_resolution_clock::time_point m_StartTime;
};
