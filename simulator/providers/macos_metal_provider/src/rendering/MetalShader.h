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

    // Represents the type of a uniform/vertex field in Metal shaders
    enum class MetalDataType {
        Int,
        Float,
        Float2,
        Float3,
        Float4,
        Mat3,
        Mat4
    };

    // Represents a single uniform field with its name and type
    struct UniformField {
        std::string name;
        MetalDataType type;
    };

    // Stores the order and types of uniform fields as defined in the shader structs
    struct UniformLayout {
        std::vector<UniformField> vertexUniforms;   // Fields from VertexUniforms struct
        std::vector<UniformField> fragmentUniforms; // Fields from FragmentUniforms struct
    };

    // Represents a single vertex attribute parsed from VertexIn struct
    struct VertexAttribute {
        std::string name;
        MetalDataType type;
        unsigned int attributeIndex;
    };

    // Stores the vertex input layout as defined in the VertexIn struct
    struct VertexLayout {
        std::vector<VertexAttribute> attributes;
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

        // Get the vertex layout parsed from the shader source
        const VertexLayout& GetVertexLayout() const { return m_vertexLayout; }

    private:
        void ParseUniformLayout(const std::string& mslSource);
        void ParseVertexLayout(const std::string& mslSource);
        static std::vector<UniformField> ParseStructFields(const std::string& source, const std::string& structName);
        static std::vector<VertexAttribute> ParseVertexInFields(const std::string& source);

        UniformLayout m_uniformLayout;
        VertexLayout m_vertexLayout;
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
