#include "MainLayer.h"
#include "ARVBase.h"
#include "utils/AssetPath.h"

#include <imgui.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

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
    m_SceneManager = std::make_unique<SceneManager>(m_Renderer, &m_State);
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
    m_ControlSection->SetTakeScreenshotCallback([this]() {
        // Extract scene name from path
        std::string sceneName = "screenshot";
        if (!m_State.currentScenePath.empty()) {
            std::string filename = m_State.currentScenePath;
            auto pos = filename.find_last_of('/');
            if (pos != std::string::npos) {
                filename = filename.substr(pos + 1);
            }
            // Remove .json extension
            auto extPos = filename.find_last_of('.');
            if (extPos != std::string::npos) {
                sceneName = filename.substr(0, extPos);
            } else {
                sceneName = filename;
            }
        }

        // Generate timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);
        std::ostringstream timestamp;
        timestamp << std::put_time(&tm, "%Y%m%d_%H%M%S");

        // Build filepath (relative to the assets directory's parent)
        std::string assetsDir = arv::AssetPath::GetAssetDirectory();
        // Go up from assets to arv-studio, then into recordings/screenshots
        std::filesystem::path basePath(assetsDir);
        std::filesystem::path screenshotDir = basePath.parent_path() / "recordings" / "screenshots";
        std::string filepath = (screenshotDir / (sceneName + "_" + timestamp.str() + ".png")).string();

        m_SceneDisplay->TakeScreenshot(filepath);
    });
    m_ControlSection->SetTakeSnapshotCallback([this]() {
        // Extract scene name from path
        std::string sceneName = "snapshot";
        if (!m_State.currentScenePath.empty()) {
            std::string filename = m_State.currentScenePath;
            auto pos = filename.find_last_of('/');
            if (pos != std::string::npos) {
                filename = filename.substr(pos + 1);
            }
            // Remove .json extension
            auto extPos = filename.find_last_of('.');
            if (extPos != std::string::npos) {
                sceneName = filename.substr(0, extPos);
            } else {
                sceneName = filename;
            }
        }

        // Generate timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);
        std::ostringstream timestamp;
        timestamp << std::put_time(&tm, "%Y%m%d_%H%M%S");

        // Build snapshot folder path
        std::string assetsDir = arv::AssetPath::GetAssetDirectory();
        std::filesystem::path basePath(assetsDir);
        std::filesystem::path snapshotDir = basePath.parent_path() / "recordings" / "snapshots" / (sceneName + "_" + timestamp.str());

        // Create the snapshot folder
        std::error_code ec;
        std::filesystem::create_directories(snapshotDir, ec);
        if (ec) {
            ARV_LOG_ERROR("Failed to create snapshot directory: {}", snapshotDir.string());
            return;
        }

        // Save screenshot
        std::string screenshotPath = (snapshotDir / "screenshot.png").string();
        m_SceneDisplay->TakeScreenshot(screenshotPath);

        // Get and save MVP matrix (View-Projection matrix)
        glm::mat4 vp = m_SceneDisplay->GetViewProjectionMatrix();
        std::string mvpPath = (snapshotDir / "mvp.csv").string();
        std::ofstream mvpFile(mvpPath);
        if (mvpFile.is_open()) {
            mvpFile << std::fixed << std::setprecision(6);
            const float* data = glm::value_ptr(vp);
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                    // GLM stores matrices in column-major order
                    mvpFile << data[col * 4 + row];
                    if (col < 3) mvpFile << ",";
                }
                mvpFile << "\n";
            }
            mvpFile.close();
            ARV_LOG_INFO("Snapshot saved to: {}", snapshotDir.string());
        } else {
            ARV_LOG_ERROR("Failed to write MVP matrix to: {}", mvpPath);
        }
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
