#if DEV_STAGE

#include "MenuBar.h"
#include "utils/Event.h"
#include <imgui/imgui.h>

namespace editor
{

void CMenuBar::Render()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("New Scene"))
      {
        // TODO: Implement new scene
      }
      if (ImGui::MenuItem("Open Scene"))
      {
        // TODO: Implement open scene
      }
      if (ImGui::MenuItem("Save Scene"))
      {
        // TODO: Implement save scene
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Exit"))
      {
        event::Notify(TEventType::Editor_RequestAppClose);
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit"))
    {
      if (ImGui::MenuItem("Undo"))
      {
        // TODO: Implement undo
      }
      if (ImGui::MenuItem("Redo"))
      {
        // TODO: Implement redo
      }
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

} // namespace editor

#endif