#include "RenderingObjectRegistration.h"
#include "rendering/RenderingObjectFactory.h"
#include "objects/ExampleTriangleRO.h"
#include "objects/SimpleTriangleRO.h"
#include "objects/ImageTextureRO.h"
#include "rendering/ObjAssetRO.h"
#include "utils/AssetPath.h"
#include <nlohmann/json.hpp>

namespace arv {

    void RegisterRenderingObjects() {
        auto& factory = RenderingObjectFactory::Instance();

        // ExampleTriangleRO
        factory.Register("ExampleTriangleRO", [](const nlohmann::json& json) -> std::unique_ptr<RenderingObject> {
            auto obj = std::make_unique<ExampleTriangleRO>();

            // Set color if provided
            if (json.contains("color") && json["color"].is_array()) {
                auto c = json.at("color");
                obj->SetColor(glm::vec4(
                    c.at(0).get<float>(),
                    c.at(1).get<float>(),
                    c.at(2).get<float>(),
                    c.at(3).get<float>()
                ));
            }

            return obj;
        });

        // SimpleTriangleRO
        factory.Register("SimpleTriangleRO", [](const nlohmann::json& json) -> std::unique_ptr<RenderingObject> {
            auto obj = std::make_unique<SimpleTriangleRO>();

            // Set color if provided
            if (json.contains("color") && json["color"].is_array()) {
                auto c = json.at("color");
                obj->SetColor(glm::vec4(
                    c.at(0).get<float>(),
                    c.at(1).get<float>(),
                    c.at(2).get<float>(),
                    c.at(3).get<float>()
                ));
            }

            return obj;
        });

        // ImageTextureRO
        factory.Register("ImageTextureRO", [](const nlohmann::json& json) -> std::unique_ptr<RenderingObject> {
            std::string texturePath;
            if (json.contains("texturePath")) {
                texturePath = json.at("texturePath").get<std::string>();
            }

            // Resolve the asset path
            return std::make_unique<ImageTextureRO>(AssetPath::Resolve(texturePath));
        });

        // ObjAssetRO
        factory.Register("ObjAssetRO", [](const nlohmann::json& json) -> std::unique_ptr<RenderingObject> {
            std::string pathFragment;
            if (json.contains("pathFragment")) {
                pathFragment = json.at("pathFragment").get<std::string>();
            }

            return std::make_unique<ObjAssetRO>(pathFragment);
        });
    }

}
