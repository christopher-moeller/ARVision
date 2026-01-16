#include "MetalShader.h"

#import <Metal/Metal.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <regex>

namespace arv {

    MetalShader::MetalShader(id<MTLDevice> device, ShaderSource* shaderSource)
        : Shader(shaderSource), m_device(device)
    {
    }

    MetalShader::~MetalShader()
    {
        Destroy();
    }

    void MetalShader::Compile()
    {
        @autoreleasepool {
            NSError* error = nil;

            // Use the MSL source from ShaderSource via GetSource()
            std::string mslSource = m_ShaderSource->GetSource("MSL_SHADER");

            // Parse the uniform layout from the shader source
            ParseUniformLayout(mslSource);

            NSString* sourceNS = [NSString stringWithUTF8String:mslSource.c_str()];

            // Compile the shader library from source
            m_library = [m_device newLibraryWithSource:sourceNS
                                               options:nil
                                                 error:&error];

            if (error)
            {
                std::cerr << "Failed to compile Metal shader library: "
                          << [[error localizedDescription] UTF8String] << std::endl;
                return;
            }

            // Get the vertex function (named "vertexMain" by convention)
            m_vertexFunction = [m_library newFunctionWithName:@"vertexMain"];
            if (!m_vertexFunction)
            {
                std::cerr << "Failed to find vertex function 'vertexMain'" << std::endl;
                return;
            }

            // Get the fragment function (named "fragmentMain" by convention)
            m_fragmentFunction = [m_library newFunctionWithName:@"fragmentMain"];
            if (!m_fragmentFunction)
            {
                std::cerr << "Failed to find fragment function 'fragmentMain'" << std::endl;
                return;
            }

            // Create the render pipeline state
            MTLRenderPipelineDescriptor* pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineDescriptor.vertexFunction = m_vertexFunction;
            pipelineDescriptor.fragmentFunction = m_fragmentFunction;
            pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

            // Configure vertex descriptor for Float3 position + Float4 color layout
            MTLVertexDescriptor* vertexDescriptor = [[MTLVertexDescriptor alloc] init];

            // Attribute 0: position (Float3)
            vertexDescriptor.attributes[0].format = MTLVertexFormatFloat3;
            vertexDescriptor.attributes[0].offset = 0;
            vertexDescriptor.attributes[0].bufferIndex = 0;

            // Attribute 1: color (Float4)
            vertexDescriptor.attributes[1].format = MTLVertexFormatFloat4;
            vertexDescriptor.attributes[1].offset = sizeof(float) * 3;  // After position
            vertexDescriptor.attributes[1].bufferIndex = 0;

            // Buffer layout: stride = 7 floats (3 for position + 4 for color)
            vertexDescriptor.layouts[0].stride = sizeof(float) * 7;
            vertexDescriptor.layouts[0].stepRate = 1;
            vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

            pipelineDescriptor.vertexDescriptor = vertexDescriptor;

            m_pipelineState = [m_device newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                                       error:&error];

            if (error)
            {
                std::cerr << "Failed to create render pipeline state: "
                          << [[error localizedDescription] UTF8String] << std::endl;
                return;
            }

            std::cout << "Metal shader compiled successfully" << std::endl;
        }
    }

    void MetalShader::Destroy()
    {
        m_pipelineState = nil;
        m_fragmentFunction = nil;
        m_vertexFunction = nil;
        m_library = nil;
    }

    void MetalShader::Use()
    {
        // In Metal, the pipeline state is set during command encoding
        // This is handled in MacosMetalRenderingAPI::Draw()
    }

    void MetalShader::UploadUniformInt(const std::string& name, int value)
    {
        m_IntUniforms[name] = value;
    }

    void MetalShader::UploadUniformFloat(const std::string& name, float value)
    {
        m_FloatUniforms[name] = value;
    }

    void MetalShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        m_Float2Uniforms[name] = value;
    }

    void MetalShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        m_Float3Uniforms[name] = value;
    }

    void MetalShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        m_Float4Uniforms[name] = value;
    }

    void MetalShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        m_Mat3Uniforms[name] = matrix;
    }

    void MetalShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        m_Mat4Uniforms[name] = matrix;
    }

    void MetalShader::ParseUniformLayout(const std::string& mslSource)
    {
        // Parse VertexUniforms struct for vertex shader uniforms
        m_uniformLayout.vertexUniforms = ParseStructFields(mslSource, "VertexUniforms");

        // Parse FragmentUniforms struct for fragment shader uniforms
        m_uniformLayout.fragmentUniforms = ParseStructFields(mslSource, "FragmentUniforms");
    }

    std::vector<UniformField> MetalShader::ParseStructFields(const std::string& source, const std::string& structName)
    {
        std::vector<UniformField> fields;

        // Find the struct definition: "struct StructName {"
        std::string structPattern = "struct\\s+" + structName + "\\s*\\{";
        std::regex structRegex(structPattern);
        std::smatch structMatch;

        if (!std::regex_search(source, structMatch, structRegex))
        {
            return fields; // Struct not found
        }

        // Find the position after the opening brace
        size_t structStart = structMatch.position() + structMatch.length();

        // Find the closing brace
        size_t braceCount = 1;
        size_t structEnd = structStart;
        while (structEnd < source.length() && braceCount > 0)
        {
            if (source[structEnd] == '{') braceCount++;
            else if (source[structEnd] == '}') braceCount--;
            structEnd++;
        }

        // Extract the struct body
        std::string structBody = source.substr(structStart, structEnd - structStart - 1);

        // Parse each field: "type fieldName;" or "type fieldName [[attribute]];"
        // Match pattern: capture type and field name
        std::regex fieldRegex(R"(\b(float4x4|float3x3|float4|float3|float2|float|int|uint)\s+(\w+)\s*(?:\[\[|;))");
        std::sregex_iterator it(structBody.begin(), structBody.end(), fieldRegex);
        std::sregex_iterator end;

        while (it != end)
        {
            std::smatch match = *it;
            std::string typeStr = match[1].str();
            std::string fieldName = match[2].str();

            UniformField field;
            field.name = fieldName;

            // Map MSL type string to MetalUniformType
            if (typeStr == "int" || typeStr == "uint")
                field.type = MetalUniformType::Int;
            else if (typeStr == "float")
                field.type = MetalUniformType::Float;
            else if (typeStr == "float2")
                field.type = MetalUniformType::Float2;
            else if (typeStr == "float3")
                field.type = MetalUniformType::Float3;
            else if (typeStr == "float4")
                field.type = MetalUniformType::Float4;
            else if (typeStr == "float3x3")
                field.type = MetalUniformType::Mat3;
            else if (typeStr == "float4x4")
                field.type = MetalUniformType::Mat4;
            else
                field.type = MetalUniformType::Float4; // Default fallback

            fields.push_back(field);
            ++it;
        }

        return fields;
    }

}
