#include "JsonSceneParser.h"
#include "rendering/RenderingObjectFactory.h"
#include "ARVBase.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

namespace arv {

    // ----- Public API -----

    ParsedScene JsonSceneParser::parseFromFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        json j;
        file >> j;

        return parseScene(j);
    }

    ParsedScene JsonSceneParser::parseFromString(const std::string& jsonText) {
        json j = json::parse(jsonText);
        return parseScene(j);
    }

    // ----- Core Scene Parsing -----

    ParsedScene JsonSceneParser::parseScene(const json& j) {
        ParsedScene scene;

        // Validate required fields
        if (!j.contains("backgroundColor") || !j["backgroundColor"].is_array()) {
            throw std::runtime_error("Scene missing 'backgroundColor'");
        }

        if (!j.contains("objects") || !j["objects"].is_array()) {
            throw std::runtime_error("Scene missing 'objects' array");
        }

        // Background color
        scene.backgroundColor = parseVec4(j.at("backgroundColor"));

        // Create objects via factory
        for (const auto& objJson : j.at("objects")) {
            auto obj = RenderingObjectFactory::Instance().Create(objJson);
            if (obj) {
                // Set position from JSON if present
                if (objJson.contains("position") && objJson["position"].is_array()) {
                    auto pos = objJson.at("position");
                    obj->SetPosition(glm::vec3(
                        pos.at(0).get<float>(),
                        pos.at(1).get<float>(),
                        pos.at(2).get<float>()
                    ));
                }

                // Set name from JSON if present
                if (objJson.contains("name")) {
                    obj->SetName(objJson.at("name").get<std::string>());
                }                               
                scene.objects.push_back(std::move(obj));
            } else {
                std::string typeName = objJson.contains("type")
                    ? objJson.at("type").get<std::string>()
                    : "unknown";
                ARV_LOG_WARN("JsonSceneParser: Failed to create object of type '{}'", typeName);
            }
        }

        return scene;
    }

    // ----- Utility Parsers -----

    glm::vec4 JsonSceneParser::parseVec4(const json& j) {
        if (!j.is_array() || j.size() != 4) {
            throw std::runtime_error("Expected vec4 array of size 4");
        }

        return glm::vec4(
            j.at(0).get<float>(),
            j.at(1).get<float>(),
            j.at(2).get<float>(),
            j.at(3).get<float>()
        );
    }

}
