#pragma once

#include "rendering/RenderingAPI.h"
#include "platform/Canvas.h"

class ImGuiManager {
public:
    ImGuiManager(arv::RenderingAPI* renderingAPI, arv::Canvas* canvas);
    ~ImGuiManager();

    void Init();
    void Shutdown();
    void BeginFrame();
    void EndFrame();

    bool IsInitialized() const { return m_Initialized; }

private:
    arv::RenderingAPI* m_RenderingAPI;
    arv::Canvas* m_Canvas;
    bool m_Initialized = false;
};
