#include "MainLayer.h"
#include "ARVBase.h"
#include "../objects/ExampleTriangleRO.h"
#include "../objects/SimpleTriangleRO.h"
#include "../events/StudioActionEvents.h"
#include "rendering/Scene.h"
#include "utils/Timestep.h"
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
#include "rendering/MetalFramebuffer.h"
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

    // Initialize ImGui first
    InitImGui();

    // Create camera with viewport dimensions (will be updated later)
    m_Camera = std::make_unique<arv::StandardCamera>(m_WindowWidth / 2, m_WindowHeight);

    // Create camera controller
    m_CameraController = arv::CreateStandardCameraController(m_Camera.get(), true);
    m_CameraController->Init();

    // Create framebuffer for scene rendering
    arv::FramebufferSpecification fbSpec;
    fbSpec.width = m_WindowWidth / 2;
    fbSpec.height = m_WindowHeight;
    fbSpec.colorAttachments = { arv::FramebufferTextureFormat::RGBA8 };
    fbSpec.hasDepthAttachment = true;
    m_SceneFramebuffer = m_RenderingAPI->CreateFramebuffer(fbSpec);

    m_ViewportSize = { static_cast<float>(fbSpec.width), static_cast<float>(fbSpec.height) };

    // Load scene from JSON file
    arv::JsonSceneParser parser;
    arv::ParsedScene parsedScene = parser.parseFromFile(
        arv::AssetPath::Resolve("scenes/main_scene.json")
    );

    m_BackgroundColor = parsedScene.backgroundColor;
    m_Objects = std::move(parsedScene.objects);

    ARV_LOG_INFO("MainLayer: Loaded {} objects from scene file", m_Objects.size());

    m_StartTime = std::chrono::high_resolution_clock::now();
}

void MainLayer::OnDetach()
{
    ARV_LOG_INFO("MainLayer::OnDetach()");
    m_Objects.clear();
    m_SceneFramebuffer.reset();
    ShutdownImGui();
}

void MainLayer::OnUpdate(float deltaTime)
{
    m_AccumulatedTime += deltaTime;

    // Animate colors for triangle objects
    float r = (std::sin(m_AccumulatedTime * 2.0f) + 1.0f) / 2.0f;
    float g = (std::sin(m_AccumulatedTime * 2.0f + 2.0f) + 1.0f) / 2.0f;
    float b = (std::sin(m_AccumulatedTime * 2.0f + 4.0f) + 1.0f) / 2.0f;

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

void MainLayer::OnRender()
{
    if (!m_ImGuiInitialized) return;

    // Render scene to framebuffer
    RenderSceneToFramebuffer();

    // Render ImGui UI
    BeginImGui();
    RenderImGuiUI();
    EndImGui();
}

void MainLayer::RenderSceneToFramebuffer()
{
    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

#ifdef __APPLE__
    if (backend == arv::RenderingBackend::Metal) {
        // For Metal, we need to start a render pass targeting the framebuffer
        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);
        metalAPI->BeginFramebufferPass(m_SceneFramebuffer, m_BackgroundColor);

        // Render scene
        arv::Scene scene = m_Renderer->NewScene(m_Camera.get());
        for (auto& object : m_Objects) {
            scene.Submit(*object);
        }
        scene.Render();

        // End framebuffer render pass
        metalAPI->EndFramebufferPass();
    }
    else
#endif
    {
        // OpenGL path
        m_SceneFramebuffer->Bind();

        m_RenderingAPI->SetClearColor(m_BackgroundColor);
        m_RenderingAPI->Clear();

        arv::Scene scene = m_Renderer->NewScene(m_Camera.get());
        for (auto& object : m_Objects) {
            scene.Submit(*object);
        }
        scene.Render();

        m_SceneFramebuffer->Unbind();
    }
}

void MainLayer::RenderImGuiUI()
{
    // Create a fullscreen dockspace-like layout
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

    // Calculate layout: left side for scene, right side for controls
    float sceneWidth = viewport->WorkSize.x * 0.65f;
    float controlsWidth = viewport->WorkSize.x * 0.35f;

    // Left panel: Scene viewport (no border, no padding for seamless scene display)
    ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::BeginChild("SceneViewport", ImVec2(sceneWidth, 0), false, childFlags);
    ImGui::PopStyleVar();
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        // Check if viewport size changed
        if (viewportSize.x != m_ViewportSize.x || viewportSize.y != m_ViewportSize.y) {
            m_ViewportSize = { viewportSize.x, viewportSize.y };
            if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0) {
                m_SceneFramebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x),
                                           static_cast<uint32_t>(m_ViewportSize.y));
                m_Camera->SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
            }
        }

        // Display the scene texture
        arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();
        if (backend == arv::RenderingBackend::OpenGL) {
            uint32_t textureID = m_SceneFramebuffer->GetColorAttachmentID();
            // Flip UV vertically for OpenGL
            ImGui::Image((ImTextureID)(intptr_t)textureID, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        }
#ifdef __APPLE__
        else if (backend == arv::RenderingBackend::Metal) {
            arv::MetalFramebuffer* metalFB = static_cast<arv::MetalFramebuffer*>(m_SceneFramebuffer.get());
            id<MTLTexture> texture = metalFB->GetColorTexture();
            ImGui::Image((__bridge ImTextureID)texture, viewportSize);
        }
#endif
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel: Controls (no horizontal scrolling)
    ImGui::BeginChild("ControlsPanel", ImVec2(controlsWidth, 0), true, childFlags);
    {
        // ARVision controls
        ImGui::Text("ARVision Controls");
        ImGui::Separator();

        if (ImGui::Button("Restart with OpenGL")) {
            arv::EventDataOnPlatformChange data;
            data.changeTo = PROVIDER_OPENGL;
            arv::CustomActionEvent event(EVENT_ON_PLATFORM_CHANGE, &data);
            arv::ARVApplication::Get()->GetEventManager()->PushEvent(event);
        }

        ImGui::SameLine();

        if (ImGui::Button("Restart with Metal")) {
            arv::EventDataOnPlatformChange data;
            data.changeTo = PROVIDER_METAL;
            arv::CustomActionEvent event(EVENT_ON_PLATFORM_CHANGE, &data);
            arv::ARVApplication::Get()->GetEventManager()->PushEvent(event);
        }

        // Show which backend is active
        arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();
        if (backend == arv::RenderingBackend::Metal) {
            ImGui::Text("Rendering Backend: Metal");
        } else if (backend == arv::RenderingBackend::OpenGL) {
            ImGui::Text("Rendering Backend: OpenGL");
        }
        
        ImGui::Separator();

        // Scene objects list
        ImGui::Text("Scene Objects (%zu)", m_Objects.size());

        if (ImGui::BeginListBox("##objectslist", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int i = 0; i < static_cast<int>(m_Objects.size()); i++)
            {
                const auto& obj = m_Objects[i];
                std::string label = obj->GetName();
                if (label.empty()) {
                    label = "Object " + std::to_string(i);
                }

                bool is_selected = (m_SelectedObjectIndex == i);
                if (ImGui::Selectable(label.c_str(), is_selected))
                {
                    m_SelectedObjectIndex = i;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        // Show selected object properties
        if (m_SelectedObjectIndex >= 0 && m_SelectedObjectIndex < static_cast<int>(m_Objects.size()))
        {
            ImGui::Separator();
            ImGui::Text("Selected Object Properties");

            auto& selectedObj = m_Objects[m_SelectedObjectIndex];
            glm::vec3 pos = selectedObj->GetPosition();

            ImGui::Text("Name: %s", selectedObj->GetName().c_str());
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
            {
                selectedObj->SetPosition(pos);
            }
        }

        ImGui::Separator();
        ImGui::Text("Viewport: %.0f x %.0f", m_ViewportSize.x, m_ViewportSize.y);

        ImGui::Separator();
    }
    ImGui::EndChild();

    ImGui::End();
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

        // During resize, drawable size may differ from ImGui's expected size
        // Update ImGui's draw data to match actual drawable dimensions to prevent scissor rect errors
        ImDrawData* drawData = ImGui::GetDrawData();
        id<CAMetalDrawable> drawable = metalAPI->GetCurrentDrawable();
        if (drawable && drawData) {
            float drawableWidth = static_cast<float>(drawable.texture.width);
            float drawableHeight = static_cast<float>(drawable.texture.height);
            float expectedWidth = drawData->DisplaySize.x * drawData->FramebufferScale.x;
            float expectedHeight = drawData->DisplaySize.y * drawData->FramebufferScale.y;

            // If there's a size mismatch, adjust the draw data
            if (std::abs(expectedWidth - drawableWidth) > 1.0f || std::abs(expectedHeight - drawableHeight) > 1.0f) {
                // Scale the framebuffer scale to match actual drawable size
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
