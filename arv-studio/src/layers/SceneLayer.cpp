#include "SceneLayer.h"
#include "../objects/ExampleTriangleRO.h"
#include "../objects/SimpleTriangleRO.h"
#include "../objects/ImageTextureRO.h"
#include "rendering/Scene.h"
#include "utils/Timestep.h"
#include <cmath>
#include <glm/glm.hpp>

SceneLayer::SceneLayer(arv::Renderer* renderer, arv::EventManager* eventManager)
    : Layer("SceneLayer"), m_Renderer(renderer), m_EventManager(eventManager)
{
}

SceneLayer::~SceneLayer()
{
}

void SceneLayer::OnAttach()
{
    // Create camera
    m_Camera = std::make_unique<arv::StandardCamera>(800, 600);

    // Create camera controller
    m_CameraController = arv::CreateStandardCameraController(m_Camera.get(), true /* isDesktop */);
    m_CameraController->Init();

    // Create scene objects
    m_TriangleObject = new arv::ExampleTriangleRO();
    m_ImageObject = new arv::ImageTextureRO("/Users/cmoeller/dev/projects/ARVision/arv-studio/assets/fc-logo.png");
    m_SimpleTriangleObject = new arv::SimpleTriangleRO();

    // Position objects
    m_TriangleObject->SetPosition(glm::vec3(-1.0f, 0.0f, 0.0f));
    m_ImageObject->SetPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    m_SimpleTriangleObject->SetPosition(glm::vec3(-1.0f, 0.0f, -1.0f));

    m_StartTime = std::chrono::high_resolution_clock::now();
}

void SceneLayer::OnDetach()
{
    delete m_ImageObject;
    delete m_TriangleObject;
    delete m_SimpleTriangleObject;

    m_ImageObject = nullptr;
    m_TriangleObject = nullptr;
    m_SimpleTriangleObject = nullptr;
}

void SceneLayer::OnUpdate(float deltaTime)
{
    m_AccumulatedTime += deltaTime;

    // Animate colors
    float r = (std::sin(m_AccumulatedTime * 2.0f) + 1.0f) / 2.0f;
    float g = (std::sin(m_AccumulatedTime * 2.0f + 2.0f) + 1.0f) / 2.0f;
    float b = (std::sin(m_AccumulatedTime * 2.0f + 4.0f) + 1.0f) / 2.0f;
    m_TriangleObject->SetColor(glm::vec4(r, g, b, 1.0f));
    m_SimpleTriangleObject->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    // Update camera controller
    arv::Timestep timestep(deltaTime);
    arv::CameraControllerAppContext context(m_EventManager, timestep);
    m_CameraController->UpdateOnStep(context);
}

void SceneLayer::OnRender()
{
    arv::Scene scene = m_Renderer->NewScene(m_Camera.get());
    scene.ClearColor({0.2f, 0.3f, 0.3f, 1.0f});
    scene.Submit(*m_TriangleObject);
    scene.Submit(*m_SimpleTriangleObject);
    scene.Submit(*m_ImageObject, m_ImageObject->GetTexture());
    scene.Render();
}
