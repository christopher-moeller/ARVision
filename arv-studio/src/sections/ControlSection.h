#pragma once

#include "rendering/RenderingObject.h"
#include "rendering/RenderingAPI.h"
#include "events/EventManager.h"
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <glm/glm.hpp>

class ControlSection {
public:
    using LoadSceneCallback = std::function<void(const std::string&)>;

    ControlSection(arv::RenderingAPI* renderingAPI,
                   std::vector<std::unique_ptr<arv::RenderingObject>>* objects,
                   int* selectedObjectIndex,
                   const glm::vec2* viewportSize);

    void SetLoadSceneCallback(LoadSceneCallback callback) { m_LoadSceneCallback = std::move(callback); }
    void SetCurrentScenePath(const std::string* path) { m_CurrentScenePath = path; }

    void RenderImGuiPanel();

private:
    arv::RenderingAPI* m_RenderingAPI;
    std::vector<std::unique_ptr<arv::RenderingObject>>* m_Objects;
    int* m_SelectedObjectIndex;
    const glm::vec2* m_ViewportSize;

    LoadSceneCallback m_LoadSceneCallback;
    const std::string* m_CurrentScenePath = nullptr;
};
