#pragma once

#include "Layer.h"
#include "platform/Canvas.h"
#include "rendering/RenderingAPI.h"
#include <memory>

class SceneEditorWindow;

class ImGuiLayer : public arv::Layer {
public:
    ImGuiLayer(arv::Canvas* canvas, arv::RenderingAPI* renderingAPI);
    ~ImGuiLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;

    // Call at the beginning of each frame before any ImGui calls
    void Begin();
    // Call at the end of each frame after all ImGui calls
    void End();

private:
    arv::Canvas* m_Canvas;
    arv::RenderingAPI* m_RenderingAPI;
    bool m_Initialized = false;

    std::unique_ptr<SceneEditorWindow> m_SceneEditorWindow;
};
