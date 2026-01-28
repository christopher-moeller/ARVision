#pragma once

#include "Layer.h"
#include "rendering/Renderer.h"
#include "rendering/RenderingAPI.h"
#include "platform/Canvas.h"
#include "events/EventManager.h"
#include "../sections/SceneDisplaySection.h"
#include "../sections/ControlSection.h"
#include "../imgui/ImGuiManager.h"
#include "../scene/SceneManager.h"
#include "../EditorState.h"
#include <memory>
#include <chrono>

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

    const EditorState& GetState() const { return m_State; }

private:
    // Core references
    arv::Renderer* m_Renderer;
    arv::Canvas* m_Canvas;
    arv::RenderingAPI* m_RenderingAPI;
    arv::EventManager* m_EventManager;
    int m_WindowWidth;
    int m_WindowHeight;

    // Shared editor state
    EditorState m_State;

    // Managers
    std::unique_ptr<ImGuiManager> m_ImGuiManager;
    std::unique_ptr<SceneManager> m_SceneManager;

    // Sections
    std::unique_ptr<SceneDisplaySection> m_SceneDisplay;
    std::unique_ptr<ControlSection> m_ControlSection;

    // Animation
    std::chrono::high_resolution_clock::time_point m_StartTime;
};
