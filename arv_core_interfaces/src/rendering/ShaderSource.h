#pragma once

#include <string>

namespace arv {

    /**
     * ShaderSource is an abstract class for providing shader source code.
     * Implementations (like CoreShaderSource) parse and provide shaders
     * for multiple rendering APIs (OpenGL GLSL and Metal MSL).
     * Each rendering backend uses GetSource() with the appropriate key
     * to retrieve its shader code at runtime.
     */
    class ShaderSource {
    public:
        ShaderSource(const std::string& rawSource) {}
        virtual ~ShaderSource() = default;

        virtual std::string GetSource(const std::string& key) = 0;
    };

}
