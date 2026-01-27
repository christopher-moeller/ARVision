#include "ControlSection.h"
#include "ARVBase.h"
#include "../events/StudioActionEvents.h"

#include <imgui.h>
#include <string>

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#endif

static std::string OpenFileDialog()
{
#ifdef __APPLE__
    @autoreleasepool {
        NSOpenPanel* panel = [NSOpenPanel openPanel];
        [panel setCanChooseFiles:YES];
        [panel setCanChooseDirectories:NO];
        [panel setAllowsMultipleSelection:NO];
        [panel setTitle:@"Select Scene File"];

        NSArray* fileTypes = @[@"json"];
        [panel setAllowedFileTypes:fileTypes];

        if ([panel runModal] == NSModalResponseOK) {
            NSURL* url = [[panel URLs] objectAtIndex:0];
            return std::string([[url path] UTF8String]);
        }
    }
#endif
    return "";
}

ControlSection::ControlSection(arv::RenderingAPI* renderingAPI,
                               std::vector<std::unique_ptr<arv::RenderingObject>>* objects,
                               int* selectedObjectIndex,
                               const glm::vec2* viewportSize)
    : m_RenderingAPI(renderingAPI)
    , m_Objects(objects)
    , m_SelectedObjectIndex(selectedObjectIndex)
    , m_ViewportSize(viewportSize)
{
}

void ControlSection::RenderImGuiPanel()
{
    ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float controlsWidth = viewport->WorkSize.x * 0.35f;

    ImGui::BeginChild("ControlsPanel", ImVec2(controlsWidth, 0), true, childFlags);
    {
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

        arv::RenderingBackend backend = m_RenderingAPI->GetBackendType();
        if (backend == arv::RenderingBackend::Metal) {
            ImGui::Text("Rendering Backend: Metal");
        } else if (backend == arv::RenderingBackend::OpenGL) {
            ImGui::Text("Rendering Backend: OpenGL");
        }

        ImGui::Separator();

        // Scene loading controls
        if (ImGui::Button("Load Scene")) {
            std::string path = OpenFileDialog();
            if (!path.empty() && m_LoadSceneCallback) {
                m_LoadSceneCallback(path);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Reload Scene")) {
            if (m_CurrentScenePath && !m_CurrentScenePath->empty() && m_LoadSceneCallback) {
                m_LoadSceneCallback(*m_CurrentScenePath);
            }
        }

        if (m_CurrentScenePath && !m_CurrentScenePath->empty()) {
            // Show just the filename
            std::string filename = *m_CurrentScenePath;
            auto pos = filename.find_last_of('/');
            if (pos != std::string::npos) {
                filename = filename.substr(pos + 1);
            }
            ImGui::Text("Scene: %s", filename.c_str());
        }

        ImGui::Separator();

        ImGui::Text("Scene Objects (%zu)", m_Objects->size());

        if (ImGui::BeginListBox("##objectslist", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int i = 0; i < static_cast<int>(m_Objects->size()); i++)
            {
                const auto& obj = (*m_Objects)[i];
                std::string label = obj->GetName();
                if (label.empty()) {
                    label = "Object " + std::to_string(i);
                }

                bool is_selected = (*m_SelectedObjectIndex == i);
                if (ImGui::Selectable(label.c_str(), is_selected))
                {
                    *m_SelectedObjectIndex = i;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        if (*m_SelectedObjectIndex >= 0 && *m_SelectedObjectIndex < static_cast<int>(m_Objects->size()))
        {
            ImGui::Separator();
            ImGui::Text("Selected Object Properties");

            auto& selectedObj = (*m_Objects)[*m_SelectedObjectIndex];
            glm::vec3 pos = selectedObj->GetPosition();

            ImGui::Text("Name: %s", selectedObj->GetName().c_str());
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
            {
                selectedObj->SetPosition(pos);
            }
        }

        ImGui::Separator();
        ImGui::Text("Viewport: %.0f x %.0f", m_ViewportSize->x, m_ViewportSize->y);

        ImGui::Separator();
    }
    ImGui::EndChild();
}
