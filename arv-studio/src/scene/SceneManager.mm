#include "SceneManager.h"
#include "ARVBase.h"
#include "utils/JsonSceneParser.h"
#include <nlohmann/json.hpp>
#include <fstream>

SceneManager::SceneManager(EditorState* state)
    : m_State(state)
{
}

void SceneManager::ApplyBackground(const std::string& mode, const glm::vec4& color, const std::string& skyboxPath)
{
    m_State->background.color = color;
    m_State->background.skyboxPath = skyboxPath;
    m_State->background.mode = (mode == "skybox")
        ? BackgroundSettings::Mode::Skybox : BackgroundSettings::Mode::Color;
}

void SceneManager::LoadScene(const std::string& path)
{
    ARV_LOG_INFO("SceneManager::LoadScene() - Loading scene from: {}", path);

    arv::JsonSceneParser parser;
    arv::ParsedScene parsedScene = parser.parseFromFile(path);

    m_State->currentScenePath = path;
    m_State->objects = std::move(parsedScene.objects);
    m_State->selectedObjectIndex = -1;

    ApplyBackground(parsedScene.backgroundMode, parsedScene.backgroundColor, parsedScene.skyboxPath);

    if (m_State->background.mode == BackgroundSettings::Mode::Skybox &&
        !m_State->background.skyboxPath.empty() && m_SkyboxLoadCallback) {
        m_SkyboxLoadCallback(m_State->background.skyboxPath);
    }

    ARV_LOG_INFO("SceneManager::LoadScene() - Loaded {} objects", m_State->objects.size());
}

void SceneManager::SaveScene()
{
    if (m_State->currentScenePath.empty()) {
        ARV_LOG_WARN("SceneManager::SaveScene() - No scene path set");
        return;
    }

    ARV_LOG_INFO("SceneManager::SaveScene() - Saving to: {}", m_State->currentScenePath);

    // Read existing JSON to preserve all fields
    std::ifstream inFile(m_State->currentScenePath);
    if (!inFile) {
        ARV_LOG_ERROR("SceneManager::SaveScene() - Failed to open file for reading: {}", m_State->currentScenePath);
        return;
    }

    nlohmann::json j;
    inFile >> j;
    inFile.close();

    // Update background settings
    nlohmann::json bgJson;
    bgJson["mode"] = (m_State->background.mode == BackgroundSettings::Mode::Skybox) ? "skybox" : "color";
    bgJson["color"] = { m_State->background.color.x, m_State->background.color.y,
                         m_State->background.color.z, m_State->background.color.w };
    bgJson["skyboxPath"] = m_State->background.skyboxPath;
    j["background"] = bgJson;

    // Update object properties in the JSON objects array
    auto& jsonObjects = j["objects"];
    for (size_t i = 0; i < m_State->objects.size() && i < jsonObjects.size(); i++) {
        const glm::vec3& pos = m_State->objects[i]->GetPosition();
        jsonObjects[i]["position"] = { pos.x, pos.y, pos.z };
        const glm::vec3& scl = m_State->objects[i]->GetScale();
        jsonObjects[i]["scale"] = { scl.x, scl.y, scl.z };
        const glm::vec3& rot = m_State->objects[i]->GetRotation();
        jsonObjects[i]["rotation"] = { rot.x, rot.y, rot.z };
        m_State->objects[i]->SaveCustomProperties(jsonObjects[i]);
    }

    // Write back
    std::ofstream outFile(m_State->currentScenePath);
    if (!outFile) {
        ARV_LOG_ERROR("SceneManager::SaveScene() - Failed to open file for writing: {}", m_State->currentScenePath);
        return;
    }

    outFile << j.dump(4) << std::endl;
    ARV_LOG_INFO("SceneManager::SaveScene() - Scene saved successfully");
}
