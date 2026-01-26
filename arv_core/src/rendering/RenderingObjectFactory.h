#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "RenderingObject.h"

namespace arv {

    // Factory function signature: takes JSON object, returns RenderingObject
    using RenderingObjectCreator = std::function<std::unique_ptr<RenderingObject>(const nlohmann::json&)>;

    class RenderingObjectFactory {
    public:
        // Get the singleton instance
        static RenderingObjectFactory& Instance();

        // Register a factory function for a type
        void Register(const std::string& typeName, RenderingObjectCreator creator);

        // Create a RenderingObject from JSON
        // Returns nullptr if type is not registered
        std::unique_ptr<RenderingObject> Create(const nlohmann::json& json) const;

        // Check if a type is registered
        bool IsRegistered(const std::string& typeName) const;

    private:
        RenderingObjectFactory() = default;
        std::unordered_map<std::string, RenderingObjectCreator> m_Creators;
    };

    // Helper macro for auto-registration (optional convenience)
    #define REGISTER_RENDERING_OBJECT(TypeName, CreatorFunc) \
        namespace { \
            struct TypeName##Registrar { \
                TypeName##Registrar() { \
                    arv::RenderingObjectFactory::Instance().Register(#TypeName, CreatorFunc); \
                } \
            }; \
            static TypeName##Registrar s_##TypeName##Registrar; \
        }

}
