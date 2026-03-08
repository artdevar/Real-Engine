#if DEV_STAGE

#include "pch.h"
#include "CollisionComponentRenderer.h"
#include "ecs/Components.h"
#include <imgui/imgui.h>

namespace editor
{
ecs::TTypeID TCollisionComponentRenderer::GetComponentTypeID() noexcept
{
  return ecs::utils::GetComponentTypeID<ecs::TCollisionComponent>();
}

void TCollisionComponentRenderer::Render(void *Data) noexcept
{
  auto *Component = static_cast<ecs::TCollisionComponent *>(Data);

  ImGui::Text("Bounding box:");
  ImGui::Text("Min: (%.2f, %.2f, %.2f)", Component->BoundingBox.Min.x, Component->BoundingBox.Min.y, Component->BoundingBox.Min.z);
  ImGui::Text("Max: (%.2f, %.2f, %.2f)", Component->BoundingBox.Max.x, Component->BoundingBox.Max.y, Component->BoundingBox.Max.z);
}

} // namespace editor

#endif