#pragma once

#include "Layer.h"
#include "camera/StandardCamera.h"
#include "camera/StandardCameraController.h"
#include "rendering/Renderer.h"
#include "events/EventManager.h"
#include <memory>
#include <chrono>

namespace arv {
    class ExampleTriangleRO;
    class SimpleTriangleRO;
    class ImageTextureRO;
}

class SceneLayer : public arv::Layer {
public:
    SceneLayer(arv::Renderer* renderer, arv::EventManager* eventManager);
    ~SceneLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;

private:
    arv::Renderer* m_Renderer;
    arv::EventManager* m_EventManager;

    std::unique_ptr<arv::StandardCamera> m_Camera;
    std::unique_ptr<arv::CameraController<arv::StandardCamera>> m_CameraController;

    arv::ExampleTriangleRO* m_TriangleObject = nullptr;
    arv::SimpleTriangleRO* m_SimpleTriangleObject = nullptr;
    arv::ImageTextureRO* m_ImageObject = nullptr;

    std::chrono::high_resolution_clock::time_point m_StartTime;
    float m_AccumulatedTime = 0.0f;
};
