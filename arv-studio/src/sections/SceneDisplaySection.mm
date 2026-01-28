#include "SceneDisplaySection.h"
#include "ARVBase.h"
#include "../objects/SelectionCubeRO.h"
#include "../objects/SkyboxRO.h"
#include "rendering/Scene.h"
#include "utils/Timestep.h"
#include "utils/AssetPath.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include "rendering/MacosMetalRenderingAPI.h"
#include "rendering/MetalFramebuffer.h"
#endif

SceneDisplaySection::SceneDisplaySection(arv::Renderer* renderer, arv::RenderingAPI* renderingAPI,
                                         arv::EventManager* eventManager, EditorState* state)
    : m_Renderer(renderer)
    , m_RenderingAPI(renderingAPI)
    , m_EventManager(eventManager)
    , m_State(state)
{
}

SceneDisplaySection::~SceneDisplaySection() = default;

void SceneDisplaySection::Init(int width, int height)
{
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

    m_Skybox = std::make_unique<arv::SkyboxRO>();
    if (m_State->background.mode == BackgroundSettings::Mode::Skybox && !m_State->background.skyboxPath.empty()) {
        LoadSkyboxTexture(m_State->background.skyboxPath);
    }
}

void SceneDisplaySection::LoadSkyboxTexture(const std::string& path)
{
    m_SkyboxTexture = m_RenderingAPI->CreateHDRTexture2D(
        arv::AssetPath::Resolve(path));
}

void SceneDisplaySection::Shutdown()
{
    m_Skybox.reset();
    m_SkyboxTexture.reset();
    m_SelectionCube.reset();
    m_SceneFramebuffer.reset();
}

void SceneDisplaySection::Update(float deltaTime)
{
    arv::Timestep timestep(deltaTime);
    arv::CameraControllerAppContext context(m_EventManager, timestep);
    m_CameraController->UpdateOnStep(context);
}

void SceneDisplaySection::RenderSkybox()
{
    if (m_State->background.mode != BackgroundSettings::Mode::Skybox || !m_Skybox || !m_SkyboxTexture)
        return;

    glm::mat4 inverseVP = glm::inverse(m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix());
    m_Skybox->GetShader()->UploadUniformMat4("u_inverseVP", inverseVP);
    m_RenderingAPI->Draw(m_Skybox->GetShader(), m_Skybox->GetVertexArray(), m_SkyboxTexture);
}

void SceneDisplaySection::SubmitScene()
{
    arv::Scene scene = m_Renderer->NewScene(m_Camera.get());
    for (auto& object : m_State->objects) {
        scene.Submit(*object);
    }
    RenderSelectionCube();
    scene.Render();
}

void SceneDisplaySection::RenderSelectionCube()
{
    if (m_State->selectedObjectIndex < 0 ||
        m_State->selectedObjectIndex >= static_cast<int>(m_State->objects.size()))
        return;

    auto& selectedObj = m_State->objects[m_State->selectedObjectIndex];
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

void SceneDisplaySection::RenderSceneToFramebuffer()
{
#ifdef __APPLE__
    if (m_RenderingAPI->GetBackendType() == arv::RenderingBackend::Metal) {
        arv::MacosMetalRenderingAPI* metalAPI = static_cast<arv::MacosMetalRenderingAPI*>(m_RenderingAPI);
        metalAPI->BeginFramebufferPass(m_SceneFramebuffer, m_State->background.color);
        RenderSkybox();
        SubmitScene();
        metalAPI->EndFramebufferPass();
    }
    else
#endif
    {
        m_SceneFramebuffer->Bind();
        m_RenderingAPI->SetClearColor(m_State->background.color);
        m_RenderingAPI->Clear();
        RenderSkybox();
        SubmitScene();
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
