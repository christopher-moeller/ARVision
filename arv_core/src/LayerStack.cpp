#include "LayerStack.h"
#include "events/Event.h"

namespace arv {

    LayerStack::~LayerStack()
    {
        for (auto& layer : m_Layers)
        {
            layer->OnDetach();
        }
    }

    void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
    {
        layer->OnAttach();
        m_Layers.insert(m_Layers.begin() + m_InsertIndex, std::move(layer));
        m_InsertIndex++;
    }

    void LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
    {
        overlay->OnAttach();
        m_Layers.push_back(std::move(overlay));
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        auto it = std::find_if(m_Layers.begin(), m_Layers.begin() + m_InsertIndex,
            [layer](const std::unique_ptr<Layer>& ptr) { return ptr.get() == layer; });

        if (it != m_Layers.begin() + m_InsertIndex)
        {
            (*it)->OnDetach();
            m_Layers.erase(it);
            m_InsertIndex--;
        }
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        auto it = std::find_if(m_Layers.begin() + m_InsertIndex, m_Layers.end(),
            [overlay](const std::unique_ptr<Layer>& ptr) { return ptr.get() == overlay; });

        if (it != m_Layers.end())
        {
            (*it)->OnDetach();
            m_Layers.erase(it);
        }
    }

    void LayerStack::OnUpdate(float deltaTime)
    {
        // Update layers from bottom to top
        for (auto& layer : m_Layers)
        {
            layer->OnUpdate(deltaTime);
        }
    }

    void LayerStack::OnRender()
    {
        // Render layers from bottom to top
        for (auto& layer : m_Layers)
        {
            layer->OnRender();
        }
    }

    void LayerStack::OnEvent(Event& event)
    {
        // Propagate events from top to bottom (overlays first)
        for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
        {
            if (event.handled)
                break;
            (*it)->OnEvent(event);
        }
    }

}
