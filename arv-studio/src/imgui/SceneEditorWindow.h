#pragma once

#include <string>

class SceneEditorWindow {
public:
    SceneEditorWindow(const std::string& title = "Scene Editor");

    void Render();

    void SetLabel(const std::string& label) { m_Label = label; }
    const std::string& GetLabel() const { return m_Label; }

    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }

private:
    std::string m_Title;
    std::string m_Label = "Hello from the Scene Editor!";
    bool m_Visible = true;
};
