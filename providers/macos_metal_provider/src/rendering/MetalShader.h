#pragma once
#include "rendering/Shader.h"
#include "rendering/ShaderSource.h"
#include <vector>

#ifdef __OBJC__
@protocol MTLDevice;
@protocol MTLRenderPipelineState;
@protocol MTLLibrary;
@protocol MTLFunction;
#else
typedef void MTLDevice;
typedef void MTLRenderPipelineState;
typedef void MTLLibrary;
typedef void MTLFunction;
#endif

namespace arv {

    // Stores the order of uniform fields as defined in the shader structs
    struct UniformLayout {
        std::vector<std::string> vertexUniformNames;   // Field names from VertexUniforms struct
        std::vector<std::string> fragmentUniformNames; // Field names from FragmentUniforms struct
    };

    class MetalShader : public Shader {

    public:
#ifdef __OBJC__
        MetalShader(id<MTLDevice> device, ShaderSource* shaderSource);
#else
        MetalShader(void* device, ShaderSource* shaderSource);
#endif

        ~MetalShader();

        void Compile() override;

        void Destroy() override;
        void Use() override;

        inline bool IsCompiled() override { return m_pipelineState != nullptr; }

        void UploadUniformInt(const std::string& name, int value) override;

        void UploadUniformFloat(const std::string& name, float value) override;
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value) override;
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value) override;
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value) override;

        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) override;
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;

#ifdef __OBJC__
        id<MTLRenderPipelineState> GetPipelineState() const { return m_pipelineState; }
#else
        void* GetPipelineState() const { return m_pipelineState; }
#endif

        // Get the uniform layout parsed from the shader source
        const UniformLayout& GetUniformLayout() const { return m_uniformLayout; }

    private:
        void ParseUniformLayout(const std::string& mslSource);
        static std::vector<std::string> ParseStructFields(const std::string& source, const std::string& structName);

        UniformLayout m_uniformLayout;
#ifdef __OBJC__
        id<MTLDevice> m_device = nullptr;
        id<MTLLibrary> m_library = nullptr;
        id<MTLFunction> m_vertexFunction = nullptr;
        id<MTLFunction> m_fragmentFunction = nullptr;
        id<MTLRenderPipelineState> m_pipelineState = nullptr;
#else
        void* m_device = nullptr;
        void* m_library = nullptr;
        void* m_vertexFunction = nullptr;
        void* m_fragmentFunction = nullptr;
        void* m_pipelineState = nullptr;
#endif
    };

}
