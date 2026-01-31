#pragma once

#include "rendering/RenderingObject.h"
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace UILayout {
    constexpr float kSceneViewportRatio = 0.65f;
    constexpr float kControlPanelRatio = 0.35f;
}

struct BackgroundSettings {
    enum class Mode { Color, Skybox };
    Mode mode = Mode::Color;
    glm::vec4 color{0.2f, 0.3f, 0.3f, 1.0f};
    std::string skyboxPath;
};

struct RecordedFrame {
    std::vector<unsigned char> pixels;
    glm::mat4 mvpMatrix;
    uint32_t width;
    uint32_t height;
};

struct RecordingState {
    bool isRecording = false;
    bool isSaving = false;
    uint32_t frameCount = 0;
    uint32_t saveProgress = 0;  // Current frame being saved
    std::string recordingPath;  // Path to the current recording folder
    std::vector<RecordedFrame> frames;  // Cached frames during recording
    glm::mat4 lastRecordedMvp{0.0f};  // Track last recorded MVP to skip duplicates
    bool hasLastMvp = false;  // Whether we have a previous MVP to compare against
};

struct EditorState {
    std::vector<std::unique_ptr<arv::RenderingObject>> objects;
    int selectedObjectIndex = -1;
    std::string currentScenePath;
    BackgroundSettings background;
    float deltaTime = 0.0f;
    RecordingState recording;
};
