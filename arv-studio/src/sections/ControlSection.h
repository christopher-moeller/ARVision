#pragma once

#include "rendering/RenderingObject.h"
#include "rendering/RenderingAPI.h"
#include "events/EventManager.h"
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <glm/glm.hpp>

struct BackgroundSettings;

class ControlSection {
public:
    using LoadSceneCallback = std::function<void(const std::string&)>;
    using SaveSceneCallback = std::function<void()>;
    using LoadSkyboxCallback = std::function<void(const std::string&)>;

    ControlSection(arv::RenderingAPI* renderingAPI,
                   std::vector<std::unique_ptr<arv::RenderingObject>>* objects,
                   int* selectedObjectIndex,
                   const glm::vec2* viewportSize);

    void SetLoadSceneCallback(LoadSceneCallback callback) { m_LoadSceneCallback = std::move(callback); }
    void SetSaveSceneCallback(SaveSceneCallback callback) { m_SaveSceneCallback = std::move(callback); }
    void SetCurrentScenePath(const std::string* path) { m_CurrentScenePath = path; }
    void SetBackgroundSettings(BackgroundSettings* bg) { m_Background = bg; }
    void SetLoadSkyboxCallback(LoadSkyboxCallback callback) { m_LoadSkyboxCallback = std::move(callback); }
    void SetDeltaTime(const float* dt) { m_DeltaTime = dt; }
    void SetMaxFPS(int* maxFPS) { m_MaxFPS = maxFPS; }

    void RenderImGuiPanel();

private:
    arv::RenderingAPI* m_RenderingAPI;
    std::vector<std::unique_ptr<arv::RenderingObject>>* m_Objects;
    int* m_SelectedObjectIndex;
    const glm::vec2* m_ViewportSize;

    LoadSceneCallback m_LoadSceneCallback;
    SaveSceneCallback m_SaveSceneCallback;
    LoadSkyboxCallback m_LoadSkyboxCallback;
    const std::string* m_CurrentScenePath = nullptr;
    BackgroundSettings* m_Background = nullptr;
    const float* m_DeltaTime = nullptr;
    int* m_MaxFPS = nullptr;
};
