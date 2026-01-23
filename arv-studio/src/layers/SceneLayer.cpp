#include "SceneLayer.h"
#include "../objects/ExampleTriangleRO.h"
#include "../objects/SimpleTriangleRO.h"
#include "../objects/ImageTextureRO.h"
#include "../objects/ObjAssetRO.h"
#include "rendering/Scene.h"
#include "utils/Timestep.h"
#include "utils/AssetPath.h"
#include <cmath>
#include <glm/glm.hpp>

SceneLayer::SceneLayer(arv::Renderer* renderer, arv::EventManager* eventManager, int width, int height)
    : Layer("SceneLayer"), m_Renderer(renderer), m_EventManager(eventManager),
      m_InitialWidth(width), m_InitialHeight(height)
{
}

SceneLayer::~SceneLayer()
{
}

void SceneLayer::OnAttach()
{
    // Create camera with the configured dimensions
    m_Camera = std::make_unique<arv::StandardCamera>(m_InitialWidth, m_InitialHeight);

    // Create camera controller
    m_CameraController = arv::CreateStandardCameraController(m_Camera.get(), true /* isDesktop */);
    m_CameraController->Init();

    // Create scene objects
    m_TriangleObject = std::make_unique<arv::ExampleTriangleRO>();
    m_ImageObject = std::make_unique<arv::ImageTextureRO>(arv::AssetPath::Resolve("fc-logo.png"));
    m_SimpleTriangleObject = std::make_unique<arv::SimpleTriangleRO>();
    m_SmgObject = std::make_unique<arv::ObjAssetRO>("SMG");

    // Position objects
    m_TriangleObject->SetPosition(glm::vec3(-1.0f, 0.0f, 0.0f));
    m_ImageObject->SetPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    m_SimpleTriangleObject->SetPosition(glm::vec3(-1.0f, 0.0f, -1.0f));
    m_SmgObject->SetPosition(glm::vec3(0.0f, 0.0f, -2.0f));

    m_StartTime = std::chrono::high_resolution_clock::now();
}

void SceneLayer::OnDetach()
{
    // unique_ptr handles cleanup automatically
    m_ImageObject.reset();
    m_TriangleObject.reset();
    m_SimpleTriangleObject.reset();
    m_SmgObject.reset();
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
    scene.Submit(*m_SmgObject, m_SmgObject->GetTexture());
    scene.Render();
}
