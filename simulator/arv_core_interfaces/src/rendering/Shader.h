#pragma once

#include <string>
#include <map>
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

        // Accessors for stored uniforms (used by rendering backends)
        // Maps are ordered alphabetically by name for deterministic buffer packing
        const std::map<std::string, int>& GetIntUniforms() const { return m_IntUniforms; }
        const std::map<std::string, float>& GetFloatUniforms() const { return m_FloatUniforms; }
        const std::map<std::string, glm::vec2>& GetFloat2Uniforms() const { return m_Float2Uniforms; }
        const std::map<std::string, glm::vec3>& GetFloat3Uniforms() const { return m_Float3Uniforms; }
        const std::map<std::string, glm::vec4>& GetFloat4Uniforms() const { return m_Float4Uniforms; }
        const std::map<std::string, glm::mat3>& GetMat3Uniforms() const { return m_Mat3Uniforms; }
        const std::map<std::string, glm::mat4>& GetMat4Uniforms() const { return m_Mat4Uniforms; }

    protected:
        ShaderSource* m_ShaderSource;
        std::map<std::string, int> m_IntUniforms;
        std::map<std::string, float> m_FloatUniforms;
        std::map<std::string, glm::vec2> m_Float2Uniforms;
        std::map<std::string, glm::vec3> m_Float3Uniforms;
        std::map<std::string, glm::vec4> m_Float4Uniforms;
        std::map<std::string, glm::mat3> m_Mat3Uniforms;
        std::map<std::string, glm::mat4> m_Mat4Uniforms;
    };

}
