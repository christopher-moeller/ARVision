#include "MainLayer.h"
#include "ARVBase.h"
#include "utils/AssetPath.h"

#include <imgui.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image_write.h>

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
        } else {
            ARV_LOG_ERROR("Failed to write MVP matrix to: {}", mvpPath);
        }

        // Export world-space vertices and indices for all scene objects
        std::vector<glm::vec3> worldVertices;
        std::vector<uint32_t> worldIndices;

        for (const auto& obj : m_State.objects) {
            const auto& localVertices = obj->GetMeshVertices();
            const auto& localIndices = obj->GetMeshIndices();

            if (localVertices.empty()) continue;

            // Compute model matrix for this object
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj->GetPosition());
            const glm::vec3& rot = obj->GetRotation();
            model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0, 0, 1));
            model = glm::scale(model, obj->GetScale());

            // Track vertex offset for index adjustment
            uint32_t vertexOffset = static_cast<uint32_t>(worldVertices.size());

            // Transform vertices to world space
            for (const auto& localPos : localVertices) {
                glm::vec4 worldPos = model * glm::vec4(localPos, 1.0f);
                worldVertices.emplace_back(worldPos.x, worldPos.y, worldPos.z);
            }

            // Add indices with offset
            for (uint32_t idx : localIndices) {
                worldIndices.push_back(vertexOffset + idx);
            }
        }

        // Write 3d_scene.obj (standard OBJ format for viewing in other apps)
        std::string objPath = (snapshotDir / "3d_scene.obj").string();
        std::ofstream objFile(objPath);
        if (objFile.is_open()) {
            objFile << "# ARVision Scene Export\n";
            objFile << "# Vertices: " << worldVertices.size() << "\n";
            objFile << "# Triangles: " << (worldIndices.size() / 3) << "\n\n";

            objFile << std::fixed << std::setprecision(6);

            // Write vertices (v x y z)
            for (const auto& v : worldVertices) {
                objFile << "v " << v.x << " " << v.y << " " << v.z << "\n";
            }

            objFile << "\n";

            // Write faces (f i1 i2 i3) - OBJ uses 1-based indexing
            for (size_t i = 0; i + 2 < worldIndices.size(); i += 3) {
                objFile << "f " << (worldIndices[i] + 1) << " "
                               << (worldIndices[i + 1] + 1) << " "
                               << (worldIndices[i + 2] + 1) << "\n";
            }

            objFile.close();
        } else {
            ARV_LOG_ERROR("Failed to write OBJ to: {}", objPath);
        }

        ARV_LOG_INFO("Snapshot saved to: {} ({} vertices, {} triangles)",
                     snapshotDir.string(), worldVertices.size(), worldIndices.size() / 3);
    });

    // Start Recording callback
    m_ControlSection->SetStartRecordingCallback([this]() {
        // Extract scene name from path
        std::string sceneName = "recording";
        if (!m_State.currentScenePath.empty()) {
            std::string filename = m_State.currentScenePath;
            auto pos = filename.find_last_of('/');
            if (pos != std::string::npos) {
                filename = filename.substr(pos + 1);
            }
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

        // Build recording folder path
        std::string assetsDir = arv::AssetPath::GetAssetDirectory();
        std::filesystem::path basePath(assetsDir);
        std::filesystem::path recordingDir = basePath.parent_path() / "recordings" / "series" / (sceneName + "_" + timestamp.str());

        // Create the recording folder
        std::error_code ec;
        std::filesystem::create_directories(recordingDir, ec);
        if (ec) {
            ARV_LOG_ERROR("Failed to create recording directory: {}", recordingDir.string());
            return;
        }

        // Set recording state
        m_State.recording.isRecording = true;
        m_State.recording.frameCount = 0;
        m_State.recording.recordingPath = recordingDir.string();
        m_State.recording.hasLastMvp = false;  // Reset MVP tracking for new recording

        ARV_LOG_INFO("Started recording to: {}", recordingDir.string());
    });

    // Stop Recording callback - triggers incremental save
    m_ControlSection->SetStopRecordingCallback([this]() {
        if (!m_State.recording.isRecording) return;

        m_State.recording.isRecording = false;
        m_State.recording.isSaving = true;
        m_State.recording.saveProgress = 0;

        ARV_LOG_INFO("Saving {} recorded frames...", m_State.recording.frames.size());
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

    // Process incremental save if saving is in progress
    if (m_State.recording.isSaving) {
        ProcessRecordingSave();
    }
}

void MainLayer::OnRender()
{
    if (!m_ImGuiManager->IsInitialized()) return;

    m_SceneDisplay->RenderSceneToFramebuffer();

    // Cache frame data in memory during recording (no disk I/O for performance)
    // Only record if MVP has changed to avoid duplicate frames
    if (m_State.recording.isRecording) {
        glm::mat4 currentMvp = m_SceneDisplay->GetViewProjectionMatrix();

        // Check if MVP has changed (compare matrices with tolerance)
        bool mvpChanged = !m_State.recording.hasLastMvp;
        if (!mvpChanged) {
            const float* curr = glm::value_ptr(currentMvp);
            const float* last = glm::value_ptr(m_State.recording.lastRecordedMvp);
            for (int i = 0; i < 16 && !mvpChanged; i++) {
                if (std::abs(curr[i] - last[i]) > 1e-6f) {
                    mvpChanged = true;
                }
            }
        }

        if (mvpChanged) {
            RecordedFrame frame;
            frame.pixels = m_SceneDisplay->CaptureFramebuffer(frame.width, frame.height);
            frame.mvpMatrix = currentMvp;

            if (!frame.pixels.empty()) {
                m_State.recording.frames.push_back(std::move(frame));
                m_State.recording.frameCount++;
                m_State.recording.lastRecordedMvp = currentMvp;
                m_State.recording.hasLastMvp = true;
            }
        }
    }

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

void MainLayer::ProcessRecordingSave()
{
    const uint32_t FRAMES_PER_UPDATE = 5;  // Save multiple frames per update for speed

    std::filesystem::path recordingDir(m_State.recording.recordingPath);
    uint32_t totalFrames = static_cast<uint32_t>(m_State.recording.frames.size());

    for (uint32_t i = 0; i < FRAMES_PER_UPDATE && m_State.recording.saveProgress < totalFrames; i++) {
        uint32_t frameIndex = m_State.recording.saveProgress;
        const auto& frame = m_State.recording.frames[frameIndex];

        // Write screenshot
        std::string screenshotPath = (recordingDir / ("screenshot_" + std::to_string(frameIndex) + ".png")).string();
        if (!frame.pixels.empty()) {
            stbi_write_png(screenshotPath.c_str(),
                           frame.width, frame.height, 4,
                           frame.pixels.data(),
                           static_cast<int>(frame.width * 4));
        }

        // Write MVP matrix
        std::string mvpPath = (recordingDir / ("mvp_" + std::to_string(frameIndex) + ".csv")).string();
        std::ofstream mvpFile(mvpPath);
        if (mvpFile.is_open()) {
            mvpFile << std::fixed << std::setprecision(6);
            const float* data = glm::value_ptr(frame.mvpMatrix);
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                    mvpFile << data[col * 4 + row];
                    if (col < 3) mvpFile << ",";
                }
                mvpFile << "\n";
            }
            mvpFile.close();
        }

        m_State.recording.saveProgress++;
    }

    // Check if save is complete
    if (m_State.recording.saveProgress >= totalFrames) {
        FinalizeSave();
    }
}

void MainLayer::FinalizeSave()
{
    std::filesystem::path recordingDir(m_State.recording.recordingPath);

    // Write 3d_scene.obj
    std::vector<glm::vec3> worldVertices;
    std::vector<uint32_t> worldIndices;

    for (const auto& obj : m_State.objects) {
        const auto& localVertices = obj->GetMeshVertices();
        const auto& localIndices = obj->GetMeshIndices();

        if (localVertices.empty()) continue;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, obj->GetPosition());
        const glm::vec3& rot = obj->GetRotation();
        model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, obj->GetScale());

        uint32_t vertexOffset = static_cast<uint32_t>(worldVertices.size());

        for (const auto& localPos : localVertices) {
            glm::vec4 worldPos = model * glm::vec4(localPos, 1.0f);
            worldVertices.emplace_back(worldPos.x, worldPos.y, worldPos.z);
        }

        for (uint32_t idx : localIndices) {
            worldIndices.push_back(vertexOffset + idx);
        }
    }

    std::string objPath = (recordingDir / "3d_scene.obj").string();
    std::ofstream objFile(objPath);
    if (objFile.is_open()) {
        objFile << "# ARVision Scene Export\n";
        objFile << "# Vertices: " << worldVertices.size() << "\n";
        objFile << "# Triangles: " << (worldIndices.size() / 3) << "\n\n";

        objFile << std::fixed << std::setprecision(6);

        for (const auto& v : worldVertices) {
            objFile << "v " << v.x << " " << v.y << " " << v.z << "\n";
        }

        objFile << "\n";

        for (size_t i = 0; i + 2 < worldIndices.size(); i += 3) {
            objFile << "f " << (worldIndices[i] + 1) << " "
                           << (worldIndices[i + 1] + 1) << " "
                           << (worldIndices[i + 2] + 1) << "\n";
        }

        objFile.close();
    }

    ARV_LOG_INFO("Recording saved: {} frames written to {}", m_State.recording.frameCount, m_State.recording.recordingPath);

    // Clean up
    m_State.recording.frames.clear();
    m_State.recording.frames.shrink_to_fit();
    m_State.recording.isSaving = false;
    m_State.recording.saveProgress = 0;
    m_State.recording.frameCount = 0;
    m_State.recording.recordingPath.clear();
    m_State.recording.hasLastMvp = false;
}
