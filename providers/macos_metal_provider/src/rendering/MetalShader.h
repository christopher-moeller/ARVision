#pragma once
#include "rendering/Shader.h"

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

    class MetalShader : public Shader {

    public:
#ifdef __OBJC__
        MetalShader(id<MTLDevice> device, const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
#else
        MetalShader(void* device, const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
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

    private:
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
