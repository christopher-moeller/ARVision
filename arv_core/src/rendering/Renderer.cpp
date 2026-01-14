#include "Renderer.h"
#include <glm/glm.hpp>

namespace arv {

    Renderer::Renderer(RenderingAPI* renderingAPI)
    {
        m_RenderingAPI.reset(renderingAPI);
    }

    void Renderer::Draw()
    {
        m_RenderingAPI->SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
        m_RenderingAPI->Clear();
        m_RenderingAPI->Draw();
    }

}
