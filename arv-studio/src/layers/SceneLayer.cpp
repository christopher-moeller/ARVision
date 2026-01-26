#include "SceneLayer.h"
#include "ARVBase.h"
#include "../objects/ExampleTriangleRO.h"
#include "../objects/SimpleTriangleRO.h"
#include "../objects/ImageTextureRO.h"
#include "rendering/ObjAssetRO.h"
#include "rendering/Scene.h"
#include "utils/Timestep.h"
#include "utils/AssetPath.h"
#include <cmath>
#include <glm/glm.hpp>
#include "utils/JsonSceneParser.h"

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
    // only a test
    JsonSceneParser parser;

     // only a test
    Scene scene = parser.parseFromFile("/Users/cmoeller/dev/projects/ARVision/arv-studio/assets/scenes/main_scene.json");
     // only a test
    for (const auto& obj : scene.objects) {
        ARV_LOG_INFO("Object of type: {}", obj->type);
    }

    // Create camera with the configured dimensions
    m_Camera = std::make_unique<arv::StandardCamera>(m_InitialWidth, m_InitialHeight);

    // Create camera controller
    m_CameraController = arv::CreateStandardCameraController(m_Camera.get(), true /* isDesktop */);
    m_CameraController->Init();

    // Create and add scene objects to vector
    auto triangle = std::make_unique<arv::ExampleTriangleRO>();
    triangle->SetPosition(glm::vec3(-1.0f, 0.0f, 0.0f));
    m_Objects.push_back(std::move(triangle));

    auto simpleTriangle = std::make_unique<arv::SimpleTriangleRO>();
    simpleTriangle->SetPosition(glm::vec3(-1.0f, 0.0f, -1.0f));
    m_Objects.push_back(std::move(simpleTriangle));

    auto image = std::make_unique<arv::ImageTextureRO>(arv::AssetPath::Resolve("fc-logo.png"));
    image->SetPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    m_Objects.push_back(std::move(image));

    auto smg = std::make_unique<arv::ObjAssetRO>("SMG");
    smg->SetPosition(glm::vec3(0.0f, 0.0f, -2.0f));
    m_Objects.push_back(std::move(smg));

    m_StartTime = std::chrono::high_resolution_clock::now();
}

void SceneLayer::OnDetach()
{
    m_Objects.clear();
}

void SceneLayer::OnUpdate(float deltaTime)
{
    m_AccumulatedTime += deltaTime;

    // Animate colors for triangle objects (first two objects)
    float r = (std::sin(m_AccumulatedTime * 2.0f) + 1.0f) / 2.0f;
    float g = (std::sin(m_AccumulatedTime * 2.0f + 2.0f) + 1.0f) / 2.0f;
    float b = (std::sin(m_AccumulatedTime * 2.0f + 4.0f) + 1.0f) / 2.0f;

    if (auto* triangle = dynamic_cast<arv::ExampleTriangleRO*>(m_Objects[0].get())) {
        triangle->SetColor(glm::vec4(r, g, b, 1.0f));
    }
    if (auto* simpleTriangle = dynamic_cast<arv::SimpleTriangleRO*>(m_Objects[1].get())) {
        simpleTriangle->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    // Update camera controller
    arv::Timestep timestep(deltaTime);
    arv::CameraControllerAppContext context(m_EventManager, timestep);
    m_CameraController->UpdateOnStep(context);
}

void SceneLayer::OnRender()
{
    arv::Scene scene = m_Renderer->NewScene(m_Camera.get());
    scene.ClearColor({0.2f, 0.3f, 0.3f, 1.0f});

    for (auto& object : m_Objects) {
        scene.Submit(*object);
    }

    scene.Render();
}
