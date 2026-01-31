#pragma once

#include <string>

namespace arv {

    class Texture2D {
    public:
        virtual ~Texture2D() = default;

        virtual void Bind(unsigned int slot = 0) const = 0;
        virtual void Unbind() const = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;
        virtual unsigned int GetChannels() const = 0;
    };

}
