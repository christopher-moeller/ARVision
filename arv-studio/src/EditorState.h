#pragma once

#include "rendering/RenderingObject.h"
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

struct BackgroundSettings {
    enum class Mode { Color, Skybox };
    Mode mode = Mode::Color;
    glm::vec4 color{0.2f, 0.3f, 0.3f, 1.0f};
    std::string skyboxPath;
};

struct EditorState {
    std::vector<std::unique_ptr<arv::RenderingObject>> objects;
    int selectedObjectIndex = -1;
    std::string currentScenePath;
    BackgroundSettings background;
    float deltaTime = 0.0f;
    int maxFPS = 0;
};
