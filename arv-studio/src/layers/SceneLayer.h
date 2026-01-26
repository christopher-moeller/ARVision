#pragma once

#include "Layer.h"
#include "camera/StandardCamera.h"
#include "camera/StandardCameraController.h"
#include "rendering/Renderer.h"
#include "rendering/RenderingObject.h"
#include "events/EventManager.h"
#include <memory>
#include <vector>
#include <chrono>
#include <glm/glm.hpp>

class SceneLayer : public arv::Layer {
public:
    SceneLayer(arv::Renderer* renderer, arv::EventManager* eventManager, int width, int height);
    ~SceneLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;

private:
    arv::Renderer* m_Renderer;
    arv::EventManager* m_EventManager;
    int m_InitialWidth;
    int m_InitialHeight;

    std::unique_ptr<arv::StandardCamera> m_Camera;
    std::unique_ptr<arv::CameraController<arv::StandardCamera>> m_CameraController;

    std::vector<std::unique_ptr<arv::RenderingObject>> m_Objects;
    glm::vec4 m_BackgroundColor{0.2f, 0.3f, 0.3f, 1.0f};

    std::chrono::high_resolution_clock::time_point m_StartTime;
    float m_AccumulatedTime = 0.0f;
};
