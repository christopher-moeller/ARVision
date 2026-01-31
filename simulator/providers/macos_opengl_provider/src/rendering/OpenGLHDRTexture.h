#pragma once

#include "rendering/Texture.h"
#include <string>

namespace arv {

    class OpenGLHDRTexture2D : public Texture2D {
    public:
        OpenGLHDRTexture2D(const std::string& path);
        ~OpenGLHDRTexture2D();

        void Bind(unsigned int slot = 0) const override;
        void Unbind() const override;

        unsigned int GetWidth() const override { return m_Width; }
        unsigned int GetHeight() const override { return m_Height; }
        unsigned int GetChannels() const override { return m_Channels; }

    private:
        unsigned int m_RendererID = 0;
        unsigned int m_Width = 0;
        unsigned int m_Height = 0;
        unsigned int m_Channels = 0;
    };

}
