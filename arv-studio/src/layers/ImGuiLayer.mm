#include "ImGuiLayer.h"
#include "ARVBase.h"

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
        // For Metal, we need to pass the render pass descriptor
        // This is a simplified version - for full Metal support,
        // we'd need to integrate with the Metal rendering pipeline

        // Create a basic render pass descriptor for ImGui
        MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        // Note: This is a simplified setup. For proper Metal integration,
        // the render pass descriptor should come from the main rendering pipeline.
        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
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
        // For Metal, we need command buffer and render encoder
        // This requires deeper integration with the Metal rendering pipeline
        // For now, Metal ImGui rendering is not fully implemented
        ARV_LOG_INFO("ImGuiLayer::End() - Metal rendering not fully implemented yet");
#endif
    }
}

void ImGuiLayer::OnRender()
{
    if (!m_Initialized) return;

    Begin();

    // Demo window for testing
    ImGui::ShowDemoWindow();

    // Simple example window
    ImGui::Begin("ARVision ImGui Example");
    ImGui::Text("Hello from ImGui!");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    static float sliderValue = 0.5f;
    ImGui::SliderFloat("Example Slider", &sliderValue, 0.0f, 1.0f);

    static int counter = 0;
    if (ImGui::Button("Click Me!")) {
        counter++;
    }
    ImGui::SameLine();
    ImGui::Text("Button clicked %d times", counter);

    ImGui::End();

    End();
}
