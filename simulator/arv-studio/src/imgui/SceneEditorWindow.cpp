#include "SceneEditorWindow.h"
#include <imgui.h>

SceneEditorWindow::SceneEditorWindow(const std::string& title)
    : m_Title(title)
{
}

void SceneEditorWindow::Render()
{
    if (!m_Visible) return;

    ImGui::Begin(m_Title.c_str(), &m_Visible);
    ImGui::Text("%s", m_Label.c_str());
    ImGui::End();
}
