#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include "rendering/RenderingObject.h"

namespace arv {

    // ----- Parsed Scene Data -----

    struct ParsedScene {
        glm::vec4 backgroundColor;
        std::vector<std::unique_ptr<RenderingObject>> objects;
    };

    // ----- Parser Class -----

    class JsonSceneParser {
    public:
        // Parse from file path - uses RenderingObjectFactory to create objects
        ParsedScene parseFromFile(const std::string& filePath);

        // Parse from already loaded JSON text
        ParsedScene parseFromString(const std::string& jsonText);

    private:
        ParsedScene parseScene(const nlohmann::json& j);
        glm::vec4 parseVec4(const nlohmann::json& j);
    };

}
