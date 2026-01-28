#pragma once

#include "rendering/RenderingObject.h"
#include "rendering/RenderingAPI.h"
#include "events/EventManager.h"
#include "../EditorState.h"
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <glm/glm.hpp>

class ControlSection {
public:
    using LoadSceneCallback = std::function<void(const std::string&)>;
    using SaveSceneCallback = std::function<void()>;
    using LoadSkyboxCallback = std::function<void(const std::string&)>;

    ControlSection(arv::RenderingAPI* renderingAPI,
                   EditorState* state,
                   const glm::vec2* viewportSize);

    void SetLoadSceneCallback(LoadSceneCallback callback) { m_LoadSceneCallback = std::move(callback); }
    void SetSaveSceneCallback(SaveSceneCallback callback) { m_SaveSceneCallback = std::move(callback); }
    void SetLoadSkyboxCallback(LoadSkyboxCallback callback) { m_LoadSkyboxCallback = std::move(callback); }

    void RenderImGuiPanel();

private:
    arv::RenderingAPI* m_RenderingAPI;
    EditorState* m_State;
    const glm::vec2* m_ViewportSize;

    LoadSceneCallback m_LoadSceneCallback;
    SaveSceneCallback m_SaveSceneCallback;
    LoadSkyboxCallback m_LoadSkyboxCallback;
};
