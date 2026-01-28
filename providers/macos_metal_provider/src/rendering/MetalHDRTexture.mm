#include "MetalHDRTexture.h"
#include "ARVBase.h"

#import <Metal/Metal.h>

#include <tinyexr.h>

#include <cmath>

// Float to half-float conversion
static uint16_t floatToHalf(float value)
{
    uint32_t f = *reinterpret_cast<uint32_t*>(&value);
    uint32_t sign = (f >> 16) & 0x8000;
    int32_t exponent = ((f >> 23) & 0xFF) - 127 + 15;
    uint32_t mantissa = f & 0x7FFFFF;

    if (exponent <= 0) {
        if (exponent < -10) return sign;
        mantissa = (mantissa | 0x800000) >> (1 - exponent);
        return sign | (mantissa >> 13);
    } else if (exponent == 0xFF - 127 + 15) {
        if (mantissa == 0) return sign | 0x7C00; // Inf
        return sign | 0x7C00 | (mantissa >> 13); // NaN
    } else if (exponent > 30) {
        return sign | 0x7C00; // Overflow to Inf
    }
    return sign | (exponent << 10) | (mantissa >> 13);
}

namespace arv {

    MetalHDRTexture2D::MetalHDRTexture2D(id<MTLDevice> device, const std::string& path)
    {
        float* rgba = nullptr;
        int width, height;
        const char* err = nullptr;

        int ret = LoadEXR(&rgba, &width, &height, path.c_str(), &err);
        if (ret != TINYEXR_SUCCESS) {
            ARV_LOG_ERROR("Failed to load EXR texture: {} ({})", path, err ? err : "unknown error");
            FreeEXRErrorMessage(err);
            return;
        }

        m_Width = width;
        m_Height = height;
        m_Channels = 4;

        // Convert float RGBA to half-float RGBA
        size_t pixelCount = static_cast<size_t>(width) * height;
        std::vector<uint16_t> halfData(pixelCount * 4);
        for (size_t i = 0; i < pixelCount * 4; i++) {
            halfData[i] = floatToHalf(rgba[i]);
        }
        free(rgba);

        // Create texture descriptor
        MTLTextureDescriptor* desc = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA16Float
            width:width
            height:height
            mipmapped:NO];
        desc.usage = MTLTextureUsageShaderRead;

        m_Texture = [device newTextureWithDescriptor:desc];
        if (!m_Texture) {
            ARV_LOG_ERROR("Failed to create Metal HDR texture");
            return;
        }

        MTLRegion region = MTLRegionMake2D(0, 0, width, height);
        [m_Texture replaceRegion:region
                     mipmapLevel:0
                       withBytes:halfData.data()
                     bytesPerRow:width * 4 * sizeof(uint16_t)];

        // Create sampler
        MTLSamplerDescriptor* samplerDesc = [[MTLSamplerDescriptor alloc] init];
        samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.sAddressMode = MTLSamplerAddressModeClampToEdge;
        samplerDesc.tAddressMode = MTLSamplerAddressModeClampToEdge;
        m_SamplerState = [device newSamplerStateWithDescriptor:samplerDesc];

        ARV_LOG_INFO("Metal HDR texture loaded: {} ({}x{})", path, width, height);
    }

    MetalHDRTexture2D::~MetalHDRTexture2D()
    {
        m_Texture = nil;
        m_SamplerState = nil;
    }

    void MetalHDRTexture2D::Bind(unsigned int slot) const
    {
        // No-op for Metal — binding done in Draw
    }

    void MetalHDRTexture2D::Unbind() const
    {
        // No-op for Metal
    }

}
