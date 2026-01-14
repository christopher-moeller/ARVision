#pragma once

#include <string>
#include <glm/glm.hpp>

namespace arv {

    class Shader {
    public:
        Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) : m_VertexShaderSource(vertexShaderSource), m_FragmentShaderSource(fragmentShaderSource) {}
        
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
        
    protected:
        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;
    };

}