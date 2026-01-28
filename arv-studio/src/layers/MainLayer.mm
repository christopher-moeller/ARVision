#include "MainLayer.h"
#include "ARVBase.h"
#include "utils/AssetPath.h"

#include <imgui.h>

MainLayer::MainLayer(arv::Renderer* renderer, arv::Canvas* canvas,
                     arv::RenderingAPI* renderingAPI, arv::EventManager* eventManager,
                     int width, int height)
    : Layer("MainLayer")
    , m_Renderer(renderer)
    , m_Canvas(canvas)
    , m_RenderingAPI(renderingAPI)
    , m_EventManager(eventManager)
    , m_WindowWidth(width)
    , m_WindowHeight(height)
{
}

MainLayer::~MainLayer()
{
}

void MainLayer::OnAttach()
{
    ARV_LOG_INFO("MainLayer::OnAttach() - Initializing");

    m_ImGuiManager = std::make_unique<ImGuiManager>(m_RenderingAPI, m_Canvas);
    m_ImGuiManager->Init();

    // Load initial scene
    m_SceneManager = std::make_unique<SceneManager>(&m_State);
    m_SceneManager->LoadScene(arv::AssetPath::Resolve("scenes/main_scene.json"));

    // Create sections
    m_SceneDisplay = std::make_unique<SceneDisplaySection>(
        m_Renderer, m_RenderingAPI, m_EventManager, &m_State);
    m_SceneDisplay->Init(m_WindowWidth, m_WindowHeight);

    m_SceneManager->SetSkyboxLoadCallback([this](const std::string& path) {
        m_SceneDisplay->LoadSkyboxTexture(path);
    });

    m_ControlSection = std::make_unique<ControlSection>(
        m_RenderingAPI, &m_State, &m_SceneDisplay->GetViewportSize());
    m_ControlSection->SetLoadSkyboxCallback([this](const std::string& path) {
        m_SceneDisplay->LoadSkyboxTexture(path);
    });
    m_ControlSection->SetLoadSceneCallback([this](const std::string& path) {
        m_SceneManager->LoadScene(path);
    });
    m_ControlSection->SetSaveSceneCallback([this]() {
        m_SceneManager->SaveScene();
    });

    m_StartTime = std::chrono::high_resolution_clock::now();
}

void MainLayer::OnDetach()
{
    ARV_LOG_INFO("MainLayer::OnDetach()");
    m_SceneDisplay->Shutdown();
    m_State.objects.clear();
    m_ImGuiManager->Shutdown();
}

void MainLayer::OnUpdate(float deltaTime)
{
    m_State.deltaTime = deltaTime;
    m_SceneDisplay->Update(deltaTime);
}

void MainLayer::OnRender()
{
    if (!m_ImGuiManager->IsInitialized()) return;

    m_SceneDisplay->RenderSceneToFramebuffer();

    m_ImGuiManager->BeginFrame();

    // Create fullscreen layout window
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                    ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar |
                                    ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("MainWindow", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    m_SceneDisplay->RenderImGuiPanel();
    ImGui::SameLine();
    m_ControlSection->RenderImGuiPanel();

    ImGui::End();

    m_ImGuiManager->EndFrame();
}
