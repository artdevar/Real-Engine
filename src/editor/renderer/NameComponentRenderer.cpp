#if DEV_STAGE

#include "NameComponentRenderer.h"
#include "ecs/Components.h"
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace editor
{
ecs::TTypeID TNameComponentRenderer::GetComponentTypeID() noexcept
{
  return ecs::utils::GetComponentTypeID<ecs::TNameComponent>();
}

void TNameComponentRenderer::Render(void *Data) noexcept
{
  auto *Component = static_cast<ecs::TNameComponent *>(Data);

  static std::string Buffer;
  static void       *CurrentData = nullptr;

  if (CurrentData != Data)
  {
    Buffer      = Component->Name;
    CurrentData = Data;
  }

  constexpr auto Flags = ImGuiInputTextFlags_NoUndoRedo;
  ImGui::InputText("Name##EntityName", &Buffer, Flags);

  if (ImGui::IsItemDeactivatedAfterEdit() && !Buffer.empty())
    Component->Name = Buffer;
}

} // namespace editor

#endif
