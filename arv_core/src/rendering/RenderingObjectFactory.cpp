#include "RenderingObjectFactory.h"
#include "ARVBase.h"

namespace arv {

    RenderingObjectFactory& RenderingObjectFactory::Instance() {
        static RenderingObjectFactory instance;
        return instance;
    }

    void RenderingObjectFactory::Register(const std::string& typeName, RenderingObjectCreator creator) {
        if (m_Creators.find(typeName) != m_Creators.end()) {
            ARV_LOG_WARN("RenderingObjectFactory: Overwriting existing creator for type '{}'", typeName);
        }
        m_Creators[typeName] = std::move(creator);
        ARV_LOG_INFO("RenderingObjectFactory: Registered type '{}'", typeName);
    }

    std::unique_ptr<RenderingObject> RenderingObjectFactory::Create(const nlohmann::json& json) const {
        if (!json.contains("type")) {
            ARV_LOG_ERROR("RenderingObjectFactory: JSON object missing 'type' field");
            return nullptr;
        }

        std::string typeName = json.at("type").get<std::string>();

        auto it = m_Creators.find(typeName);
        if (it == m_Creators.end()) {
            ARV_LOG_ERROR("RenderingObjectFactory: Unknown type '{}'", typeName);
            return nullptr;
        }

        return it->second(json);
    }

    bool RenderingObjectFactory::IsRegistered(const std::string& typeName) const {
        return m_Creators.find(typeName) != m_Creators.end();
    }

}
