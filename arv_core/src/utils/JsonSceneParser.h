#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include "rendering/RenderingObject.h"

namespace arv {

    class Renderer;

    // ----- Parsed Scene Data -----

    struct ParsedScene {
        glm::vec4 backgroundColor;
        std::string backgroundMode; // "color" or "skybox"
        std::string skyboxPath;
        std::vector<std::unique_ptr<RenderingObject>> objects;
    };

    // ----- Parser Class -----

    class JsonSceneParser {
    public:
        // Parse from file path - uses RenderingObjectFactory to create objects
        ParsedScene parseFromFile(Renderer* renderer, const std::string& filePath);

        // Parse from already loaded JSON text
        ParsedScene parseFromString(Renderer* renderer, const std::string& jsonText);

    private:
        ParsedScene parseScene(Renderer* renderer, const nlohmann::json& j);
        glm::vec4 parseVec4(const nlohmann::json& j);
    };

}
