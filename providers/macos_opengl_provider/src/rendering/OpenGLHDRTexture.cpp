#include "OpenGLHDRTexture.h"
#include "ARVBase.h"
#include <glad/glad.h>

#include <tinyexr.h>

namespace arv {

    OpenGLHDRTexture2D::OpenGLHDRTexture2D(const std::string& path)
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
        m_Channels = 4; // LoadEXR always returns RGBA

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, rgba);

        free(rgba);

        ARV_LOG_INFO("OpenGL HDR texture loaded: {} ({}x{})", path, width, height);
    }

    OpenGLHDRTexture2D::~OpenGLHDRTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLHDRTexture2D::Bind(unsigned int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void OpenGLHDRTexture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}
