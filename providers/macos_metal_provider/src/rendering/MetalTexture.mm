#include "MetalTexture.h"

#import <Metal/Metal.h>
#include <stb_image.h>
#include <iostream>

namespace arv {

    MetalTexture2D::MetalTexture2D(id<MTLDevice> device, const std::string& path)
    {
        // Don't flip for Metal (Metal expects top-left origin)
        stbi_set_flip_vertically_on_load(false);

        int width, height, channels;
        // Force RGBA by requesting 4 channels
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

        if (!data)
        {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return;
        }

        m_Width = width;
        m_Height = height;
        m_Channels = 4; // We forced RGBA

        // Create texture descriptor
        MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                     width:width
                                                                                                    height:height
                                                                                                 mipmapped:NO];
        textureDescriptor.usage = MTLTextureUsageShaderRead;

        // Create the texture
        m_Texture = [device newTextureWithDescriptor:textureDescriptor];

        if (!m_Texture)
        {
            std::cerr << "Failed to create Metal texture" << std::endl;
            stbi_image_free(data);
            return;
        }

        // Copy image data to texture
        MTLRegion region = MTLRegionMake2D(0, 0, width, height);
        [m_Texture replaceRegion:region
                     mipmapLevel:0
                       withBytes:data
                     bytesPerRow:width * 4];

        stbi_image_free(data);

        // Create sampler state
        MTLSamplerDescriptor* samplerDescriptor = [[MTLSamplerDescriptor alloc] init];
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;

        m_SamplerState = [device newSamplerStateWithDescriptor:samplerDescriptor];

        std::cout << "Metal texture loaded: " << path << " (" << width << "x" << height << ", " << m_Channels << " channels)" << std::endl;
    }

    MetalTexture2D::~MetalTexture2D()
    {
        m_Texture = nil;
        m_SamplerState = nil;
    }

    void MetalTexture2D::Bind(unsigned int slot) const
    {
        // In Metal, texture binding is handled during render encoding
        // This is a no-op as the binding happens in MacosMetalRenderingAPI::Draw
    }

    void MetalTexture2D::Unbind() const
    {
        // No-op for Metal
    }

}
