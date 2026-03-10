#if DEV_STAGE

#include "SkyboxComponentRenderer.h"
#include "ecs/Components.h"
#include "assets/Texture.h"
#include <imgui/imgui.h>

namespace editor
{
ecs::TTypeID TSkyboxComponentRenderer::GetComponentTypeID() noexcept
{
  return ecs::utils::GetComponentTypeID<ecs::TEnvironmentComponent>();
}

void TSkyboxComponentRenderer::Render(void *Data) noexcept
{
  auto *Component = static_cast<ecs::TEnvironmentComponent *>(Data);
  ImGui::Text("Path: %s", Component->EquirectangularMap ? Component->EquirectangularMap->GetPath().string().c_str() : "Not loaded");
}

} // namespace editor

#endif