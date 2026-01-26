#include "SceneLayer.h"
#include "ARVBase.h"
#include "../objects/ExampleTriangleRO.h"
#include "../objects/SimpleTriangleRO.h"
#include "rendering/Scene.h"
#include "utils/Timestep.h"
#include "utils/AssetPath.h"
#include "utils/JsonSceneParser.h"
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

    // Load scene from JSON file
    arv::JsonSceneParser parser;
    arv::ParsedScene parsedScene = parser.parseFromFile(
        arv::AssetPath::Resolve("scenes/main_scene.json")
    );

    // Store background color
    m_BackgroundColor = parsedScene.backgroundColor;

    // Move objects from parsed scene
    m_Objects = std::move(parsedScene.objects);

    ARV_LOG_INFO("SceneLayer: Loaded {} objects from scene file", m_Objects.size());

    m_StartTime = std::chrono::high_resolution_clock::now();
}

void SceneLayer::OnDetach()
{
    m_Objects.clear();
}

void SceneLayer::OnUpdate(float deltaTime)
{
    m_AccumulatedTime += deltaTime;

    // Animate colors for triangle objects
    float r = (std::sin(m_AccumulatedTime * 2.0f) + 1.0f) / 2.0f;
    float g = (std::sin(m_AccumulatedTime * 2.0f + 2.0f) + 1.0f) / 2.0f;
    float b = (std::sin(m_AccumulatedTime * 2.0f + 4.0f) + 1.0f) / 2.0f;

    // Find and animate triangle objects
    for (auto& obj : m_Objects) {
        if (auto* triangle = dynamic_cast<arv::ExampleTriangleRO*>(obj.get())) {
            triangle->SetColor(glm::vec4(r, g, b, 1.0f));
        }
    }

    // Update camera controller
    arv::Timestep timestep(deltaTime);
    arv::CameraControllerAppContext context(m_EventManager, timestep);
    m_CameraController->UpdateOnStep(context);
}

void SceneLayer::OnRender()
{
    arv::Scene scene = m_Renderer->NewScene(m_Camera.get());
    scene.ClearColor(m_BackgroundColor);

    for (auto& object : m_Objects) {
        scene.Submit(*object);
    }

    scene.Render();
}
