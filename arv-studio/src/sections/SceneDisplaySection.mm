#include "SceneDisplaySection.h"
#include "ARVBase.h"
#include "../objects/SelectionCubeRO.h"
#include "rendering/Scene.h"
#include "utils/Timestep.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include "rendering/MacosMetalRenderingAPI.h"
#include "rendering/MetalFramebuffer.h"
#endif

SceneDisplaySection::SceneDisplaySection(arv::Renderer* renderer, arv::RenderingAPI* renderingAPI,
                                         arv::EventManager* eventManager,
                                         std::vector<std::unique_ptr<arv::RenderingObject>>* objects,
                                         int* selectedObjectIndex)
    : m_Renderer(renderer)
    , m_RenderingAPI(renderingAPI)
    , m_EventManager(eventManager)
    , m_Objects(objects)
    , m_SelectedObjectIndex(selectedObjectIndex)
{
}

SceneDisplaySection::~SceneDisplaySection() = default;

void SceneDisplaySection::Init(int width, int height, const glm::vec4& backgroundColor)
{
    m_BackgroundColor = backgroundColor;

    m_Camera = std::make_unique<arv::StandardCamera>(width / 2, height);

    m_CameraController = arv::CreateStandardCameraController(m_Camera.get(), true);
    m_CameraController->Init();

    arv::FramebufferSpecification fbSpec;
    fbSpec.width = width / 2;
    fbSpec.height = height;
    fbSpec.colorAttachments = { arv::FramebufferTextureFormat::RGBA8 };
    fbSpec.hasDepthAttachment = true;
    m_SceneFramebuffer = m_RenderingAPI->CreateFramebuffer(fbSpec);

    m_ViewportSize = { static_cast<float>(fbSpec.width), static_cast<float>(fbSpec.height) };

    m_SelectionCube = std::make_unique<arv::SelectionCubeRO>();
}

void SceneDisplaySection::Shutdown()
{
    m_SelectionCube.reset();
    m_SceneFramebuffer.reset();
}

void SceneDisplaySection::Update(float deltaTime)
{
    arv::Timestep timestep(deltaTime);
    arv::CameraControllerAppContext context(m_EventManager, timestep);
    m_CameraController->UpdateOnStep(context);
}

void SceneDisplaySection::RenderSceneToFramebuffer()
{
    arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();

#ifdef __APPLE__
    if (backend == arv::RenderingBackend::Metal) {
        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);
        metalAPI->BeginFramebufferPass(m_SceneFramebuffer, m_BackgroundColor);

        arv::Scene scene = m_Renderer->NewScene(m_Camera.get());
        for (auto& object : *m_Objects) {
            scene.Submit(*object);
        }

        // Render selection cube overlay scaled to object's bounding box
        if (*m_SelectedObjectIndex >= 0 && *m_SelectedObjectIndex < static_cast<int>(m_Objects->size())) {
            auto& selectedObj = (*m_Objects)[*m_SelectedObjectIndex];
            glm::vec3 objPos = selectedObj->GetPosition();
            glm::vec3 boundsCenter = selectedObj->GetBoundsCenter();
            glm::vec3 boundsSize = selectedObj->GetBoundsSize();

            glm::mat4 projection = m_Camera->GetProjectionMatrix();
            glm::mat4 view = m_Camera->GetViewMatrix();
            const glm::vec3& rot = selectedObj->GetRotation();
            const glm::vec3& scl = selectedObj->GetScale();
            glm::mat4 model = glm::translate(glm::mat4(1.0f), objPos);
            model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0, 0, 1));
            model = glm::translate(model, boundsCenter * scl);
            model = glm::scale(model, boundsSize * scl);
            glm::mat4 mvp = projection * view * model;

            m_SelectionCube->GetShader()->UploadUniformMat4("u_mvp", mvp);
            m_RenderingAPI->Draw(m_SelectionCube->GetShader(), m_SelectionCube->GetVertexArray());
        }

        scene.Render();

        metalAPI->EndFramebufferPass();
    }
    else
#endif
    {
        m_SceneFramebuffer->Bind();

        m_RenderingAPI->SetClearColor(m_BackgroundColor);
        m_RenderingAPI->Clear();

        arv::Scene scene = m_Renderer->NewScene(m_Camera.get());
        for (auto& object : *m_Objects) {
            scene.Submit(*object);
        }

        // Render selection cube overlay scaled to object's bounding box
        if (*m_SelectedObjectIndex >= 0 && *m_SelectedObjectIndex < static_cast<int>(m_Objects->size())) {
            auto& selectedObj = (*m_Objects)[*m_SelectedObjectIndex];
            glm::vec3 objPos = selectedObj->GetPosition();
            glm::vec3 boundsCenter = selectedObj->GetBoundsCenter();
            glm::vec3 boundsSize = selectedObj->GetBoundsSize();

            glm::mat4 projection = m_Camera->GetProjectionMatrix();
            glm::mat4 view = m_Camera->GetViewMatrix();
            const glm::vec3& rot = selectedObj->GetRotation();
            const glm::vec3& scl = selectedObj->GetScale();
            glm::mat4 model = glm::translate(glm::mat4(1.0f), objPos);
            model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0, 0, 1));
            model = glm::translate(model, boundsCenter * scl);
            model = glm::scale(model, boundsSize * scl);
            glm::mat4 mvp = projection * view * model;

            m_SelectionCube->GetShader()->UploadUniformMat4("u_mvp", mvp);
            m_RenderingAPI->Draw(m_SelectionCube->GetShader(), m_SelectionCube->GetVertexArray());
        }

        scene.Render();

        m_SceneFramebuffer->Unbind();
    }
}

void SceneDisplaySection::RenderImGuiPanel()
{
    ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float sceneWidth = viewport->WorkSize.x * 0.65f;

    ImGui::BeginChild("SceneViewport", ImVec2(sceneWidth, 0), false, childFlags);
    ImGui::PopStyleVar();
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        if (viewportSize.x != m_ViewportSize.x || viewportSize.y != m_ViewportSize.y) {
            m_ViewportSize = { viewportSize.x, viewportSize.y };
            if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0) {
                m_SceneFramebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x),
                                           static_cast<uint32_t>(m_ViewportSize.y));
                m_Camera->SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
            }
        }

        arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();
        if (backend == arv::RenderingBackend::OpenGL) {
            uint32_t textureID = m_SceneFramebuffer->GetColorAttachmentID();
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
}
