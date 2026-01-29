#pragma once

#include "../EditorState.h"
#include <string>
#include <functional>

namespace arv { class Renderer; }

class SceneManager {
public:
    using SkyboxLoadCallback = std::function<void(const std::string&)>;

    SceneManager(arv::Renderer* renderer, EditorState* state);

    void SetSkyboxLoadCallback(SkyboxLoadCallback callback) { m_SkyboxLoadCallback = std::move(callback); }

    void LoadScene(const std::string& path);
    void SaveScene();

private:
    void ApplyBackground(const std::string& mode, const glm::vec4& color, const std::string& skyboxPath);

    arv::Renderer* m_Renderer;
    EditorState* m_State;
    SkyboxLoadCallback m_SkyboxLoadCallback;
};
