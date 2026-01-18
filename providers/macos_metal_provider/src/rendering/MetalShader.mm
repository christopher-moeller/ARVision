#include "MetalShader.h"

#import <Metal/Metal.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <regex>
#include <algorithm>

namespace arv {

    MetalShader::MetalShader(id<MTLDevice> device, ShaderSource* shaderSource)
        : Shader(shaderSource), m_device(device)
    {
    }

    MetalShader::~MetalShader()
    {
        Destroy();
    }

    // Helper function to convert MetalDataType to MTLVertexFormat
    static MTLVertexFormat MetalDataTypeToVertexFormat(MetalDataType type)
    {
        switch (type)
        {
            case MetalDataType::Float:  return MTLVertexFormatFloat;
            case MetalDataType::Float2: return MTLVertexFormatFloat2;
            case MetalDataType::Float3: return MTLVertexFormatFloat3;
            case MetalDataType::Float4: return MTLVertexFormatFloat4;
            default: return MTLVertexFormatFloat4;
        }
    }

    // Helper function to get size in bytes for MetalDataType
    static unsigned int MetalDataTypeSize(MetalDataType type)
    {
        switch (type)
        {
            case MetalDataType::Float:  return 4;
            case MetalDataType::Float2: return 8;
            case MetalDataType::Float3: return 12;
            case MetalDataType::Float4: return 16;
            case MetalDataType::Mat3:   return 36;
            case MetalDataType::Mat4:   return 64;
            case MetalDataType::Int:    return 4;
            default: return 16;
        }
    }

    void MetalShader::Compile()
    {
        @autoreleasepool {
            NSError* error = nil;

            // Use the MSL source from ShaderSource via GetSource()
            std::string mslSource = m_ShaderSource->GetSource("MSL_SHADER");

            // Parse the uniform layout from the shader source
            ParseUniformLayout(mslSource);

            // Parse the vertex layout from the shader source
            ParseVertexLayout(mslSource);

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

            // Enable alpha blending for transparent textures
            pipelineDescriptor.colorAttachments[0].blendingEnabled = YES;
            pipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
            pipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
            pipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
            pipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

            // Build vertex descriptor dynamically from parsed vertex layout
            MTLVertexDescriptor* vertexDescriptor = [[MTLVertexDescriptor alloc] init];
            unsigned int currentOffset = 0;

            for (const auto& attr : m_vertexLayout.attributes)
            {
                vertexDescriptor.attributes[attr.attributeIndex].format = MetalDataTypeToVertexFormat(attr.type);
                vertexDescriptor.attributes[attr.attributeIndex].offset = currentOffset;
                vertexDescriptor.attributes[attr.attributeIndex].bufferIndex = 0;

                currentOffset += MetalDataTypeSize(attr.type);
            }

            // Buffer layout: stride = total size of all attributes
            vertexDescriptor.layouts[0].stride = currentOffset;
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

            // Map MSL type string to MetalDataType
            if (typeStr == "int" || typeStr == "uint")
                field.type = MetalDataType::Int;
            else if (typeStr == "float")
                field.type = MetalDataType::Float;
            else if (typeStr == "float2")
                field.type = MetalDataType::Float2;
            else if (typeStr == "float3")
                field.type = MetalDataType::Float3;
            else if (typeStr == "float4")
                field.type = MetalDataType::Float4;
            else if (typeStr == "float3x3")
                field.type = MetalDataType::Mat3;
            else if (typeStr == "float4x4")
                field.type = MetalDataType::Mat4;
            else
                field.type = MetalDataType::Float4; // Default fallback

            fields.push_back(field);
            ++it;
        }

        return fields;
    }

    void MetalShader::ParseVertexLayout(const std::string& mslSource)
    {
        m_vertexLayout.attributes = ParseVertexInFields(mslSource);
    }

    std::vector<VertexAttribute> MetalShader::ParseVertexInFields(const std::string& source)
    {
        std::vector<VertexAttribute> attributes;

        // Find the VertexIn struct definition
        std::string structPattern = "struct\\s+VertexIn\\s*\\{";
        std::regex structRegex(structPattern);
        std::smatch structMatch;

        if (!std::regex_search(source, structMatch, structRegex))
        {
            return attributes; // VertexIn struct not found
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

        // Parse each field with attribute index: "type fieldName [[attribute(N)]];"
        // Match pattern: capture type, field name, and attribute index
        std::regex fieldRegex(R"(\b(float4|float3|float2|float|int|uint)\s+(\w+)\s*\[\[attribute\((\d+)\)\]\])");
        std::sregex_iterator it(structBody.begin(), structBody.end(), fieldRegex);
        std::sregex_iterator end;

        while (it != end)
        {
            std::smatch match = *it;
            std::string typeStr = match[1].str();
            std::string fieldName = match[2].str();
            unsigned int attrIndex = std::stoul(match[3].str());

            VertexAttribute attr;
            attr.name = fieldName;
            attr.attributeIndex = attrIndex;

            // Map MSL type string to MetalDataType
            if (typeStr == "int" || typeStr == "uint")
                attr.type = MetalDataType::Int;
            else if (typeStr == "float")
                attr.type = MetalDataType::Float;
            else if (typeStr == "float2")
                attr.type = MetalDataType::Float2;
            else if (typeStr == "float3")
                attr.type = MetalDataType::Float3;
            else if (typeStr == "float4")
                attr.type = MetalDataType::Float4;
            else
                attr.type = MetalDataType::Float4; // Default fallback

            attributes.push_back(attr);
            ++it;
        }

        // Sort by attribute index to ensure correct offset calculation
        std::sort(attributes.begin(), attributes.end(),
            [](const VertexAttribute& a, const VertexAttribute& b) {
                return a.attributeIndex < b.attributeIndex;
            });

        return attributes;
    }

}
