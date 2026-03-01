#if DEV_STAGE

#include "ModelComponentRenderer.h"
#include "ecs/Components.h"
#include <imgui/imgui.h>

namespace editor
{
ecs::TTypeID TModelComponentRenderer::GetComponentTypeID() noexcept
{
  return ecs::utils::GetComponentTypeID<ecs::TModelComponent>();
}

void TModelComponentRenderer::Render(void *Data) noexcept
{
  auto *Component = static_cast<ecs::TModelComponent *>(Data);
  ImGui::Text("Primitives count: %d", Component->Primitives.size());
  ImGui::Text("Materials count: %d", Component->Materials.size());
}

} // namespace editor

#endif