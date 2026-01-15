#pragma once

#include <memory>
#include "rendering/Shader.h"
#include "rendering/VertexArray.h"

namespace arv {

    class RenderingObject {
        
    public:
        virtual std::shared_ptr<Shader>& GetShader() = 0;
        virtual std::shared_ptr<VertexArray>& GetVertexArray() = 0;
        
    };

}
