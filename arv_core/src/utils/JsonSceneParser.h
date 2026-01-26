#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

// ----- Math Types -----

struct Vec3 {
    float x, y, z;
};

struct Vec4 {
    float r, g, b, a;
};

// ----- Scene Object Hierarchy -----

struct ObjectBase {
    std::string type;
    Vec3 position;
    virtual ~ObjectBase() = default;
};

struct ImageObject : ObjectBase {
    std::string texturePath;
};

struct TriangleObject : ObjectBase {
    Vec4 color;
};

// ----- Scene -----

struct Scene {
    Vec4 backgroundColor;
    std::vector<std::unique_ptr<ObjectBase>> objects;
};

// ----- Parser Class -----

class JsonSceneParser {
public:
    // Parse from file path
    Scene parseFromFile(const std::string& filePath);

    // Parse from already loaded JSON text
    Scene parseFromString(const std::string& jsonText);

private:
    // Core parsing helpers
    Scene parseScene(const nlohmann::json& j);
    std::unique_ptr<ObjectBase> parseObject(const nlohmann::json& j);

    // Small utility parsers
    Vec3 parseVec3(const nlohmann::json& j);
    Vec4 parseVec4(const nlohmann::json& j);
};
