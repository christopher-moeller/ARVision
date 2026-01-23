#include "ImGuiLayer.h"
#include "ARVBase.h"

#include "../events/StudioActionEvents.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_metal.h>

#include <GLFW/glfw3.h>

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include "MacosMetalPlatformProvider.h"
#include "rendering/MacosMetalRenderingAPI.h"
#include "platform/MacosMetalGlfwCanvas.h"
#endif

ImGuiLayer::ImGuiLayer(arv::Canvas* canvas, arv::RenderingAPI* renderingAPI)
    : Layer("ImGuiLayer")
    , m_Canvas(canvas)
    , m_RenderingAPI(renderingAPI)
{
}

ImGuiLayer::~ImGuiLayer()
{
}

void ImGuiLayer::OnAttach()
{
    ARV_LOG_INFO("ImGuiLayer::OnAttach() - Initializing ImGui");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Get the GLFW window
    GLFWwindow* window = static_cast<GLFWwindow*>(m_Canvas->GetNativeWindow());
    if (!window) {
        ARV_LOG_ERROR("ImGuiLayer::OnAttach() - Failed to get native window");
        return;
    }

    // Setup Platform/Renderer backends based on the rendering API type
    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

    if (backend == arv::RenderingBackend::OpenGL) {
        ARV_LOG_INFO("ImGuiLayer::OnAttach() - Initializing OpenGL3 backend");
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        m_Initialized = true;
    }
    else if (backend == arv::RenderingBackend::Metal) {
#ifdef __APPLE__
        ARV_LOG_INFO("ImGuiLayer::OnAttach() - Initializing Metal backend");
        ImGui_ImplGlfw_InitForOther(window, true);

        // Get the Metal device from the rendering API
        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);
        id<MTLDevice> device = metalAPI->GetDevice();

        if (device) {
            ImGui_ImplMetal_Init(device);
            m_Initialized = true;
            ARV_LOG_INFO("ImGuiLayer::OnAttach() - Metal backend initialized successfully");
        } else {
            ARV_LOG_ERROR("ImGuiLayer::OnAttach() - Failed to get Metal device");
        }
#else
        ARV_LOG_ERROR("ImGuiLayer::OnAttach() - Metal backend is only supported on Apple platforms");
#endif
    }
    else {
        ARV_LOG_ERROR("ImGuiLayer::OnAttach() - Unknown rendering backend");
    }

    ARV_LOG_INFO("ImGuiLayer::OnAttach() - ImGui initialized successfully");
}

void ImGuiLayer::OnDetach()
{
    ARV_LOG_INFO("ImGuiLayer::OnDetach() - Shutting down ImGui");

    if (!m_Initialized) return;

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

void ImGuiLayer::OnUpdate(float deltaTime)
{
    // ImGui doesn't need per-frame updates outside of rendering
}

void ImGuiLayer::Begin()
{
    if (!m_Initialized) return;

    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

    if (backend == arv::RenderingBackend::OpenGL) {
        ImGui_ImplOpenGL3_NewFrame();
    }
    else if (backend == arv::RenderingBackend::Metal) {
#ifdef __APPLE__
        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);

        // End the current scene render pass so ImGui can create its own
        metalAPI->EndRenderPass();

        // Create render pass descriptor for ImGui (loads existing content)
        MTLRenderPassDescriptor* renderPassDescriptor = metalAPI->CreateImGuiRenderPassDescriptor();

        if (renderPassDescriptor) {
            ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        }
#endif
    }

    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    if (!m_Initialized) return;

    ImGui::Render();

    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

    if (backend == arv::RenderingBackend::OpenGL) {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    else if (backend == arv::RenderingBackend::Metal) {
#ifdef __APPLE__
        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);

        // Get the render pass descriptor for ImGui
        MTLRenderPassDescriptor* renderPassDescriptor = metalAPI->CreateImGuiRenderPassDescriptor();
        if (!renderPassDescriptor) {
            ARV_LOG_ERROR("ImGuiLayer::End() - Failed to create render pass descriptor");
            return;
        }

        // Create a render encoder for ImGui
        id<MTLRenderCommandEncoder> renderEncoder = metalAPI->CreateImGuiRenderEncoder(renderPassDescriptor);
        if (!renderEncoder) {
            ARV_LOG_ERROR("ImGuiLayer::End() - Failed to create render encoder");
            return;
        }

        // Get the command buffer for ImGui rendering
        id<MTLCommandBuffer> commandBuffer = metalAPI->GetCurrentCommandBuffer();
        if (!commandBuffer) {
            ARV_LOG_ERROR("ImGuiLayer::End() - No current command buffer");
            metalAPI->EndImGuiRenderPass(renderEncoder);
            return;
        }

        // Render ImGui draw data
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

        // End the ImGui render pass
        metalAPI->EndImGuiRenderPass(renderEncoder);
#endif
    }
}

void ImGuiLayer::OnRender()
{
    if (!m_Initialized) return;

    Begin();

    // Demo window for testing
    //ImGui::ShowDemoWindow();

    // Simple example window
    ImGui::Begin("ARVision");

    if(ImGui::Button("Restart with OpenGL")) {
        arv::EventDataOnPlatformChange data;
        data.changeTo = PROVIDER_OPENGL;
        arv::CustomActionEvent event(EVENT_ON_PLATFORM_CHANGE, &data);
        arv::ARVApplication::Get()->GetEventManager()->PushEvent(event);
    }
    
    ImGui::SameLine();
    
    if(ImGui::Button("Restart with Metal")) {
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

    ImGui::End();

    End();
}
