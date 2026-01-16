#include "MetalShader.h"

#import <Metal/Metal.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

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
        // Metal uses buffer bindings for uniforms
        // This would require maintaining a uniform buffer
        // For now, this is a placeholder
    }

    void MetalShader::UploadUniformFloat(const std::string& name, float value)
    {
        // Metal uses buffer bindings for uniforms
    }

    void MetalShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        // Metal uses buffer bindings for uniforms
    }

    void MetalShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        // Metal uses buffer bindings for uniforms
    }

    void MetalShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        // Store the uniform value - will be applied during Draw()
        m_Float4Uniforms[name] = value;
    }

    void MetalShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        // Metal uses buffer bindings for uniforms
    }

    void MetalShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        // Metal uses buffer bindings for uniforms
    }

}
