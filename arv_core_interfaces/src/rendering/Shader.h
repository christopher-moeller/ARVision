#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "ShaderSource.h"

namespace arv {

    class Shader {
    public:
        Shader(ShaderSource* shaderSource) : m_ShaderSource(shaderSource) {}

        virtual ~Shader() = default;

        virtual void Compile() = 0;
        virtual bool IsCompiled() = 0;

        virtual void Destroy() = 0;
        virtual void Use() = 0;

        virtual void UploadUniformInt(const std::string& name, int value) = 0;

        virtual void UploadUniformFloat(const std::string& name, float value) = 0;
        virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

        virtual void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) = 0;
        virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0;

        // Accessors for stored uniforms (used by Metal rendering)
        const std::unordered_map<std::string, glm::vec4>& GetFloat4Uniforms() const { return m_Float4Uniforms; }

    protected:
        ShaderSource* m_ShaderSource;
        std::unordered_map<std::string, glm::vec4> m_Float4Uniforms;
        std::unordered_map<std::string, glm::mat4> m_Mat4Uniforms;
    };

}
