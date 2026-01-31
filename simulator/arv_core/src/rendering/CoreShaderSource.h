#pragma once

#include <unordered_map>

#include "rendering/ShaderSource.h"

namespace arv {

    class CoreShaderSource : public ShaderSource {
    public:
        CoreShaderSource(const std::string& rawSource);
        ~CoreShaderSource() = default;

        std::string GetSource(const std::string& key) override;

    private:
        std::string m_RawSource;
        std::unordered_map<std::string, std::string> m_ParsedShaders;
        std::unordered_map<std::string, std::string> parseShaders(const std::string& source);
    };

}