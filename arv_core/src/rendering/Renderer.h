#pragma once

#include <memory>
#include "rendering/RenderingAPI.h"

namespace arv {

    class Renderer {
    public:
        Renderer(RenderingAPI* renderingAPI);
        void Draw();

    private:
        std::shared_ptr<RenderingAPI> m_RenderingAPI;
    };

}
