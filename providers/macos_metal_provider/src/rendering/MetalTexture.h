#pragma once

#include "rendering/Texture.h"
#include <string>

#ifdef __OBJC__
@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLSamplerState;
#else
typedef void MTLDevice;
typedef void MTLTexture;
typedef void MTLSamplerState;
#endif

namespace arv {

    class MetalTexture2D : public Texture2D {
    public:
#ifdef __OBJC__
        MetalTexture2D(id<MTLDevice> device, const std::string& path);
#else
        MetalTexture2D(void* device, const std::string& path);
#endif
        ~MetalTexture2D();

        void Bind(unsigned int slot = 0) const override;
        void Unbind() const override;

        unsigned int GetWidth() const override { return m_Width; }
        unsigned int GetHeight() const override { return m_Height; }
        unsigned int GetChannels() const override { return m_Channels; }

#ifdef __OBJC__
        id<MTLTexture> GetMetalTexture() const { return m_Texture; }
        id<MTLSamplerState> GetSamplerState() const { return m_SamplerState; }
#else
        void* GetMetalTexture() const { return m_Texture; }
        void* GetSamplerState() const { return m_SamplerState; }
#endif

    private:
#ifdef __OBJC__
        id<MTLTexture> m_Texture = nullptr;
        id<MTLSamplerState> m_SamplerState = nullptr;
#else
        void* m_Texture = nullptr;
        void* m_SamplerState = nullptr;
#endif
        unsigned int m_Width = 0;
        unsigned int m_Height = 0;
        unsigned int m_Channels = 0;
    };

}
