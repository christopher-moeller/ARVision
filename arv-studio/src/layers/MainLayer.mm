#include "MainLayer.h"
#include "ARVBase.h"
#include "../objects/SimpleTriangleRO.h"
#include "utils/AssetPath.h"
#include "utils/JsonSceneParser.h"
#include <nlohmann/json.hpp>
#include <fstream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_metal.h>
#include <GLFW/glfw3.h>
#include <cmath>

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include "MacosMetalPlatformProvider.h"
#include "rendering/MacosMetalRenderingAPI.h"
#include "platform/MacosMetalGlfwCanvas.h"
#endif

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

    InitImGui();

    // Load scene from JSON file
    arv::JsonSceneParser parser;
    arv::ParsedScene parsedScene = parser.parseFromFile(
        arv::AssetPath::Resolve("scenes/main_scene.json")
    );

    m_State.currentScenePath = arv::AssetPath::Resolve("scenes/main_scene.json");
    m_State.objects = std::move(parsedScene.objects);
    ARV_LOG_INFO("MainLayer: Loaded {} objects from scene file", m_State.objects.size());

    // Apply background settings from parsed scene
    m_State.background.color = parsedScene.backgroundColor;
    m_State.background.skyboxPath = parsedScene.skyboxPath;
    m_State.background.mode = (parsedScene.backgroundMode == "skybox")
        ? BackgroundSettings::Mode::Skybox : BackgroundSettings::Mode::Color;

    // Create sections
    m_SceneDisplay = std::make_unique<SceneDisplaySection>(
        m_Renderer, m_RenderingAPI, m_EventManager, &m_State);
    m_SceneDisplay->Init(m_WindowWidth, m_WindowHeight);

    m_ControlSection = std::make_unique<ControlSection>(
        m_RenderingAPI, &m_State, &m_SceneDisplay->GetViewportSize());
    m_ControlSection->SetLoadSkyboxCallback([this](const std::string& path) {
        m_SceneDisplay->LoadSkyboxTexture(path);
    });
    m_ControlSection->SetLoadSceneCallback([this](const std::string& path) {
        LoadScene(path);
    });
    m_ControlSection->SetSaveSceneCallback([this]() {
        SaveScene();
    });

    m_StartTime = std::chrono::high_resolution_clock::now();
}

void MainLayer::OnDetach()
{
    ARV_LOG_INFO("MainLayer::OnDetach()");
    m_SceneDisplay->Shutdown();
    m_State.objects.clear();
    ShutdownImGui();
}

void MainLayer::LoadScene(const std::string& path)
{
    ARV_LOG_INFO("MainLayer::LoadScene() - Loading scene from: {}", path);

    arv::JsonSceneParser parser;
    arv::ParsedScene parsedScene = parser.parseFromFile(path);

    m_State.currentScenePath = path;
    m_State.objects = std::move(parsedScene.objects);
    m_State.selectedObjectIndex = -1;

    m_State.background.color = parsedScene.backgroundColor;
    m_State.background.skyboxPath = parsedScene.skyboxPath;
    m_State.background.mode = (parsedScene.backgroundMode == "skybox")
        ? BackgroundSettings::Mode::Skybox : BackgroundSettings::Mode::Color;

    if (m_State.background.mode == BackgroundSettings::Mode::Skybox && !m_State.background.skyboxPath.empty()) {
        m_SceneDisplay->LoadSkyboxTexture(m_State.background.skyboxPath);
    }

    ARV_LOG_INFO("MainLayer::LoadScene() - Loaded {} objects", m_State.objects.size());
}

void MainLayer::SaveScene()
{
    if (m_State.currentScenePath.empty()) {
        ARV_LOG_WARN("MainLayer::SaveScene() - No scene path set");
        return;
    }

    ARV_LOG_INFO("MainLayer::SaveScene() - Saving to: {}", m_State.currentScenePath);

    // Read existing JSON to preserve all fields
    std::ifstream inFile(m_State.currentScenePath);
    if (!inFile) {
        ARV_LOG_ERROR("MainLayer::SaveScene() - Failed to open file for reading: {}", m_State.currentScenePath);
        return;
    }

    nlohmann::json j;
    inFile >> j;
    inFile.close();

    // Update background settings
    nlohmann::json bgJson;
    bgJson["mode"] = (m_State.background.mode == BackgroundSettings::Mode::Skybox) ? "skybox" : "color";
    bgJson["color"] = { m_State.background.color.x, m_State.background.color.y,
                         m_State.background.color.z, m_State.background.color.w };
    bgJson["skyboxPath"] = m_State.background.skyboxPath;
    j["background"] = bgJson;

    // Update positions in the JSON objects array
    auto& jsonObjects = j["objects"];
    for (size_t i = 0; i < m_State.objects.size() && i < jsonObjects.size(); i++) {
        const glm::vec3& pos = m_State.objects[i]->GetPosition();
        jsonObjects[i]["position"] = { pos.x, pos.y, pos.z };
        const glm::vec3& scl = m_State.objects[i]->GetScale();
        jsonObjects[i]["scale"] = { scl.x, scl.y, scl.z };
        const glm::vec3& rot = m_State.objects[i]->GetRotation();
        jsonObjects[i]["rotation"] = { rot.x, rot.y, rot.z };
        if (auto* tri = dynamic_cast<arv::SimpleTriangleRO*>(m_State.objects[i].get())) {
            const glm::vec4& col = tri->GetColor();
            jsonObjects[i]["color"] = { col.x, col.y, col.z, col.w };
        }
    }

    // Write back
    std::ofstream outFile(m_State.currentScenePath);
    if (!outFile) {
        ARV_LOG_ERROR("MainLayer::SaveScene() - Failed to open file for writing: {}", m_State.currentScenePath);
        return;
    }

    outFile << j.dump(4) << std::endl;
    ARV_LOG_INFO("MainLayer::SaveScene() - Scene saved successfully");
}

void MainLayer::OnUpdate(float deltaTime)
{
    m_State.deltaTime = deltaTime;
    m_SceneDisplay->Update(deltaTime);
}

void MainLayer::OnRender()
{
    if (!m_ImGuiInitialized) return;

    m_SceneDisplay->RenderSceneToFramebuffer();

    BeginImGui();

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

    EndImGui();
}

void MainLayer::InitImGui()
{
    ARV_LOG_INFO("MainLayer::InitImGui() - Initializing ImGui");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    GLFWwindow* window = static_cast<GLFWwindow*>(m_Canvas->GetNativeWindow());
    if (!window) {
        ARV_LOG_ERROR("MainLayer::InitImGui() - Failed to get native window");
        return;
    }

    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

    if (backend == arv::RenderingBackend::OpenGL) {
        ARV_LOG_INFO("MainLayer::InitImGui() - Initializing OpenGL3 backend");
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        m_ImGuiInitialized = true;
    }
    else if (backend == arv::RenderingBackend::Metal) {
#ifdef __APPLE__
        ARV_LOG_INFO("MainLayer::InitImGui() - Initializing Metal backend");
        ImGui_ImplGlfw_InitForOther(window, true);

        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);
        id<MTLDevice> device = metalAPI->GetDevice();

        if (device) {
            ImGui_ImplMetal_Init(device);
            m_ImGuiInitialized = true;
            ARV_LOG_INFO("MainLayer::InitImGui() - Metal backend initialized successfully");
        } else {
            ARV_LOG_ERROR("MainLayer::InitImGui() - Failed to get Metal device");
        }
#endif
    }
}

void MainLayer::ShutdownImGui()
{
    if (!m_ImGuiInitialized) return;

    ARV_LOG_INFO("MainLayer::ShutdownImGui()");

    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

    if (backend == arv::RenderingBackend::OpenGL) {
        ImGui_ImplOpenGL3_Shutdown();
    }
    else if (backend == arv::RenderingBackend::Metal) {
#ifdef __APPLE__
        ImGui_ImplMetal_Shutdown();
#endif
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void MainLayer::BeginImGui()
{
    if (!m_ImGuiInitialized) return;

    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

    if (backend == arv::RenderingBackend::OpenGL) {
        ImGui_ImplOpenGL3_NewFrame();
    }
    else if (backend == arv::RenderingBackend::Metal) {
#ifdef __APPLE__
        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);
        metalAPI->EndRenderPass();
        MTLRenderPassDescriptor* renderPassDescriptor = metalAPI->CreateImGuiRenderPassDescriptor();
        if (renderPassDescriptor) {
            ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        }
#endif
    }

    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void MainLayer::EndImGui()
{
    if (!m_ImGuiInitialized) return;

    ImGui::Render();

    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

    if (backend == arv::RenderingBackend::OpenGL) {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    else if (backend == arv::RenderingBackend::Metal) {
#ifdef __APPLE__
        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);

        MTLRenderPassDescriptor* renderPassDescriptor = metalAPI->CreateImGuiRenderPassDescriptor();
        if (!renderPassDescriptor) {
            ARV_LOG_ERROR("MainLayer::EndImGui() - Failed to create render pass descriptor");
            return;
        }

        id<MTLRenderCommandEncoder> renderEncoder = metalAPI->CreateImGuiRenderEncoder(renderPassDescriptor);
        if (!renderEncoder) {
            ARV_LOG_ERROR("MainLayer::EndImGui() - Failed to create render encoder");
            return;
        }

        id<MTLCommandBuffer> commandBuffer = metalAPI->GetCurrentCommandBuffer();
        if (!commandBuffer) {
            ARV_LOG_ERROR("MainLayer::EndImGui() - No current command buffer");
            metalAPI->EndImGuiRenderPass(renderEncoder);
            return;
        }

        ImDrawData* drawData = ImGui::GetDrawData();
        id<CAMetalDrawable> drawable = metalAPI->GetCurrentDrawable();
        if (drawable && drawData) {
            float drawableWidth = static_cast<float>(drawable.texture.width);
            float drawableHeight = static_cast<float>(drawable.texture.height);
            float expectedWidth = drawData->DisplaySize.x * drawData->FramebufferScale.x;
            float expectedHeight = drawData->DisplaySize.y * drawData->FramebufferScale.y;

            if (std::abs(expectedWidth - drawableWidth) > 1.0f || std::abs(expectedHeight - drawableHeight) > 1.0f) {
                if (drawData->DisplaySize.x > 0 && drawData->DisplaySize.y > 0) {
                    drawData->FramebufferScale.x = drawableWidth / drawData->DisplaySize.x;
                    drawData->FramebufferScale.y = drawableHeight / drawData->DisplaySize.y;
                }
            }
        }

        ImGui_ImplMetal_RenderDrawData(drawData, commandBuffer, renderEncoder);
        metalAPI->EndImGuiRenderPass(renderEncoder);
#endif
    }
}
