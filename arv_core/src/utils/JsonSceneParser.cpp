#include "JsonSceneParser.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>

using json = nlohmann::json;

// ----- Public API -----

Scene JsonSceneParser::parseFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    json j;
    file >> j;

    return parseScene(j);
}

Scene JsonSceneParser::parseFromString(const std::string& jsonText) {
    json j = json::parse(jsonText);
    return parseScene(j);
}

// ----- Core Scene Parsing -----

Scene JsonSceneParser::parseScene(const json& j) {
    Scene scene;

    // Validate required fields
    if (!j.contains("backgroundColor") || !j["backgroundColor"].is_array()) {
        throw std::runtime_error("Scene missing 'backgroundColor'");
    }

    if (!j.contains("objects") || !j["objects"].is_array()) {
        throw std::runtime_error("Scene missing 'objects' array");
    }

    // Background
    scene.backgroundColor = parseVec4(j.at("backgroundColor"));

    // Objects
    for (const auto& objJson : j.at("objects")) {
        scene.objects.push_back(parseObject(objJson));
    }

    return scene;
}

// ----- Object Factory -----

std::unique_ptr<ObjectBase> JsonSceneParser::parseObject(const json& j) {
    if (!j.contains("type")) {
        throw std::runtime_error("Object missing 'type' field");
    }

    std::string type = j.at("type").get<std::string>();

    if (!j.contains("position")) {
        throw std::runtime_error("Object of type '" + type + "' missing 'position'");
    }

    if (type == "image") {
        if (!j.contains("texturePath")) {
            throw std::runtime_error("Image object missing 'texturePath'");
        }

        auto obj = std::make_unique<ImageObject>();
        obj->type = type;
        obj->position = parseVec3(j.at("position"));
        obj->texturePath = j.at("texturePath").get<std::string>();
        return obj;
    }
    else if (type == "simple-triangle") {
        if (!j.contains("color")) {
            throw std::runtime_error("Triangle object missing 'color'");
        }

        auto obj = std::make_unique<TriangleObject>();
        obj->type = type;
        obj->position = parseVec3(j.at("position"));
        obj->color = parseVec4(j.at("color"));
        return obj;
    }
    else {
        throw std::runtime_error("Unknown object type: " + type);
    }
}

// ----- Utility Parsers -----

Vec3 JsonSceneParser::parseVec3(const json& j) {
    if (!j.is_array() || j.size() != 3) {
        throw std::runtime_error("Expected Vec3 array of size 3");
    }

    return {
        j.at(0).get<float>(),
        j.at(1).get<float>(),
        j.at(2).get<float>()
    };
}

Vec4 JsonSceneParser::parseVec4(const json& j) {
    if (!j.is_array() || j.size() != 4) {
        throw std::runtime_error("Expected Vec4 array of size 4");
    }

    return {
        j.at(0).get<float>(),
        j.at(1).get<float>(),
        j.at(2).get<float>(),
        j.at(3).get<float>()
    };
}
