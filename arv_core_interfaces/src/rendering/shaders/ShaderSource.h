#pragma once

#include <string>


namespace arv {

    struct ShaderSourceData {
        std::string vertexShaderSource;
        std::string fragmentShaderSource;
    };

    class ShaderSource {
    public:
        virtual ~ShaderSource() = default;
        virtual ShaderSourceData GetOpenGLShader() const = 0;
        virtual ShaderSourceData GetOpenGLESShader() const = 0;
    };

}