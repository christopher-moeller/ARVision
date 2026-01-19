#pragma once

#include "Layer.h"
#include <vector>
#include <memory>

namespace arv {

    class LayerStack {
    public:
        LayerStack() = default;
        ~LayerStack();

        void PushLayer(std::unique_ptr<Layer> layer);
        void PushOverlay(std::unique_ptr<Layer> overlay);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);

        void OnUpdate(float deltaTime);
        void OnRender();
        void OnEvent(Event& event);

        // Iterators for external access
        auto begin() { return m_Layers.begin(); }
        auto end() { return m_Layers.end(); }
        auto rbegin() { return m_Layers.rbegin(); }
        auto rend() { return m_Layers.rend(); }

    private:
        std::vector<std::unique_ptr<Layer>> m_Layers;
        size_t m_InsertIndex = 0;  // Overlays go after this index
    };

}
