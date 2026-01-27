#pragma once

#include "rendering/RenderingObject.h"
#include "rendering/RenderingAPI.h"
#include "events/EventManager.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class ControlSection {
public:
    ControlSection(arv::RenderingAPI* renderingAPI,
                   std::vector<std::unique_ptr<arv::RenderingObject>>* objects,
                   int* selectedObjectIndex,
                   const glm::vec2* viewportSize);

    void RenderImGuiPanel();

private:
    arv::RenderingAPI* m_RenderingAPI;
    std::vector<std::unique_ptr<arv::RenderingObject>>* m_Objects;
    int* m_SelectedObjectIndex;
    const glm::vec2* m_ViewportSize;
};
