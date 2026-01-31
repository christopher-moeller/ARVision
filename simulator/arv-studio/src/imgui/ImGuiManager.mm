#include "ImGuiManager.h"
#include "ARVBase.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_metal.h>
#include <GLFW/glfw3.h>
#include <cmath>

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include "rendering/MacosMetalRenderingAPI.h"
#endif

ImGuiManager::ImGuiManager(arv::RenderingAPI* renderingAPI, arv::Canvas* canvas)
    : m_RenderingAPI(renderingAPI)
    , m_Canvas(canvas)
{
}

ImGuiManager::~ImGuiManager()
{
    Shutdown();
}

void ImGuiManager::Init()
{
    ARV_LOG_INFO("ImGuiManager::Init()");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    GLFWwindow* window = static_cast<GLFWwindow*>(m_Canvas->GetNativeWindow());
    if (!window) {
        ARV_LOG_ERROR("ImGuiManager::Init() - Failed to get native window");
        return;
    }

    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

    if (backend == arv::RenderingBackend::OpenGL) {
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        m_Initialized = true;
    }
    else if (backend == arv::RenderingBackend::Metal) {
#ifdef __APPLE__
        ImGui_ImplGlfw_InitForOther(window, true);

        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);
        id<MTLDevice> device = metalAPI->GetDevice();

        if (device) {
            ImGui_ImplMetal_Init(device);
            m_Initialized = true;
        } else {
            ARV_LOG_ERROR("ImGuiManager::Init() - Failed to get Metal device");
        }
#endif
    }
}

void ImGuiManager::Shutdown()
{
    if (!m_Initialized) return;

    ARV_LOG_INFO("ImGuiManager::Shutdown()");

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
    m_Initialized = false;
}

void ImGuiManager::BeginFrame()
{
    if (!m_Initialized) return;

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

void ImGuiManager::EndFrame()
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

        MTLRenderPassDescriptor* renderPassDescriptor = metalAPI->CreateImGuiRenderPassDescriptor();
        if (!renderPassDescriptor) {
            ARV_LOG_ERROR("ImGuiManager::EndFrame() - Failed to create render pass descriptor");
            return;
        }

        id<MTLRenderCommandEncoder> renderEncoder = metalAPI->CreateImGuiRenderEncoder(renderPassDescriptor);
        if (!renderEncoder) {
            ARV_LOG_ERROR("ImGuiManager::EndFrame() - Failed to create render encoder");
            return;
        }

        id<MTLCommandBuffer> commandBuffer = metalAPI->GetCurrentCommandBuffer();
        if (!commandBuffer) {
            ARV_LOG_ERROR("ImGuiManager::EndFrame() - No current command buffer");
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
