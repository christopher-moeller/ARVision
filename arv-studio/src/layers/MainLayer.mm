#include "MainLayer.h"
#include "ARVBase.h"
#include "utils/AssetPath.h"
#include "utils/JsonSceneParser.h"

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

    m_CurrentScenePath = arv::AssetPath::Resolve("scenes/main_scene.json");
    m_Objects = std::move(parsedScene.objects);
    ARV_LOG_INFO("MainLayer: Loaded {} objects from scene file", m_Objects.size());

    // Create sections
    m_SceneDisplay = std::make_unique<SceneDisplaySection>(
        m_Renderer, m_RenderingAPI, m_EventManager, &m_Objects, &m_SelectedObjectIndex);
    m_SceneDisplay->Init(m_WindowWidth, m_WindowHeight, parsedScene.backgroundColor);

    m_ControlSection = std::make_unique<ControlSection>(
        m_RenderingAPI, &m_Objects, &m_SelectedObjectIndex, &m_SceneDisplay->GetViewportSize());
    m_ControlSection->SetCurrentScenePath(&m_CurrentScenePath);
    m_ControlSection->SetLoadSceneCallback([this](const std::string& path) {
        LoadScene(path);
    });

    m_StartTime = std::chrono::high_resolution_clock::now();
}

void MainLayer::OnDetach()
{
    ARV_LOG_INFO("MainLayer::OnDetach()");
    m_SceneDisplay->Shutdown();
    m_Objects.clear();
    ShutdownImGui();
}

void MainLayer::LoadScene(const std::string& path)
{
    ARV_LOG_INFO("MainLayer::LoadScene() - Loading scene from: {}", path);

    arv::JsonSceneParser parser;
    arv::ParsedScene parsedScene = parser.parseFromFile(path);

    m_CurrentScenePath = path;
    m_Objects = std::move(parsedScene.objects);
    m_SelectedObjectIndex = -1;
    m_SceneDisplay->SetBackgroundColor(parsedScene.backgroundColor);

    ARV_LOG_INFO("MainLayer::LoadScene() - Loaded {} objects", m_Objects.size());
}

void MainLayer::OnUpdate(float deltaTime)
{
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
