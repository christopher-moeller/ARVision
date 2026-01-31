#include "LayerStack.h"
#include "ARVBase.h"
#include "events/Event.h"

namespace arv {

    LayerStack::~LayerStack()
    {
        ARV_LOG_INFO("LayerStack::~LayerStack() - Destroying LayerStack with {} layers", m_Layers.size());
        for (auto& layer : m_Layers)
        {
            ARV_LOG_INFO("LayerStack::~LayerStack() - Detaching layer: {}", layer->GetName());
            layer->OnDetach();
        }
    }

    void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
    {
        ARV_LOG_INFO("LayerStack::PushLayer() - Pushing layer: {}", layer->GetName());
        layer->OnAttach();
        m_Layers.insert(m_Layers.begin() + m_InsertIndex, std::move(layer));
        m_InsertIndex++;
        ARV_LOG_INFO("LayerStack::PushLayer() - Layer stack now has {} layers ({} regular, {} overlays)",
                     m_Layers.size(), m_InsertIndex, m_Layers.size() - m_InsertIndex);
    }

    void LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
    {
        ARV_LOG_INFO("LayerStack::PushOverlay() - Pushing overlay: {}", overlay->GetName());
        overlay->OnAttach();
        m_Layers.push_back(std::move(overlay));
        ARV_LOG_INFO("LayerStack::PushOverlay() - Layer stack now has {} layers ({} regular, {} overlays)",
                     m_Layers.size(), m_InsertIndex, m_Layers.size() - m_InsertIndex);
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        ARV_LOG_INFO("LayerStack::PopLayer() - Attempting to pop layer: {}", layer->GetName());
        auto it = std::find_if(m_Layers.begin(), m_Layers.begin() + m_InsertIndex,
            [layer](const std::unique_ptr<Layer>& ptr) { return ptr.get() == layer; });

        if (it != m_Layers.begin() + m_InsertIndex)
        {
            ARV_LOG_INFO("LayerStack::PopLayer() - Layer found, detaching: {}", layer->GetName());
            (*it)->OnDetach();
            m_Layers.erase(it);
            m_InsertIndex--;
        }
        else
        {
            ARV_LOG_INFO("LayerStack::PopLayer() - Layer not found in regular layers");
        }
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        ARV_LOG_INFO("LayerStack::PopOverlay() - Attempting to pop overlay: {}", overlay->GetName());
        auto it = std::find_if(m_Layers.begin() + m_InsertIndex, m_Layers.end(),
            [overlay](const std::unique_ptr<Layer>& ptr) { return ptr.get() == overlay; });

        if (it != m_Layers.end())
        {
            ARV_LOG_INFO("LayerStack::PopOverlay() - Overlay found, detaching: {}", overlay->GetName());
            (*it)->OnDetach();
            m_Layers.erase(it);
        }
        else
        {
            ARV_LOG_INFO("LayerStack::PopOverlay() - Overlay not found");
        }
    }

    void LayerStack::OnUpdate(float deltaTime)
    {
        for (auto& layer : m_Layers)
        {
            layer->OnUpdate(deltaTime);
        }
    }

    void LayerStack::OnRender()
    {
        for (auto& layer : m_Layers)
        {
            layer->OnRender();
        }
    }

    void LayerStack::OnEvent(Event& event)
    {
        for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
        {
            if (event.handled)
                break;
            (*it)->OnEvent(event);
        }
    }

}
