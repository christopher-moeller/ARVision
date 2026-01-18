#include "OpenGLTexture.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

namespace arv {

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
    {
        // Flip for OpenGL (OpenGL texture origin is bottom-left)
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (!data)
        {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return;
        }

        m_Width = width;
        m_Height = height;
        m_Channels = channels;

        GLenum internalFormat = 0;
        GLenum dataFormat = 0;

        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }
        else if (channels == 1)
        {
            internalFormat = GL_R8;
            dataFormat = GL_RED;
        }

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);

        std::cout << "OpenGL texture loaded: " << path << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::Bind(unsigned int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void OpenGLTexture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}
