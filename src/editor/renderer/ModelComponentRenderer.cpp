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
  auto  *Component     = static_cast<ecs::TModelComponent *>(Data);
  size_t TotalVertices = 0;
  size_t TotalIndices  = 0;

  for (const auto &Primitive : Component->Primitives)
  {
    TotalVertices += Primitive.VerticesCount;
    TotalIndices  += Primitive.IndicesCount;
  }

  ImGui::Text("Primitives count: %d", Component->Primitives.size());
  ImGui::Text("Materials count: %d", Component->Materials.size());
  ImGui::Separator();
  ImGui::Text("Total vertices count: %d", TotalVertices);
  ImGui::Text("Total indices count: %d", TotalIndices);
}

} // namespace editor

#endif