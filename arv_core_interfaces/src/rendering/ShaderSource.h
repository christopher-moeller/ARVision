#pragma once

#include <string>

namespace arv {

    /**
     * ShaderSource holds shader source code for multiple rendering APIs.
     * This allows RenderingObjects to provide both GLSL (OpenGL) and MSL (Metal)
     * shaders, enabling the abstraction layer to select the appropriate shader
     * at runtime based on the active rendering backend.
     */
    struct ShaderSource {
        // OpenGL GLSL shaders
        std::string glslVertexSource;
        std::string glslFragmentSource;

        // Metal MSL shader (vertex and fragment combined as per Metal convention)
        std::string mslSource;

        ShaderSource() = default;

        ShaderSource(const std::string& glslVertex, const std::string& glslFragment, const std::string& msl)
            : glslVertexSource(glslVertex), glslFragmentSource(glslFragment), mslSource(msl) {}
    };

}
