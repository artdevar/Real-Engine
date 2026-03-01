#if DEV_STAGE

#include "SkyboxComponentRenderer.h"
#include "ecs/Components.h"
#include "assets/Texture.h"
#include <imgui/imgui.h>

namespace editor
{
ecs::TTypeID TSkyboxComponentRenderer::GetComponentTypeID() noexcept
{
  return ecs::utils::GetComponentTypeID<ecs::TSkyboxComponent>();
}

void TSkyboxComponentRenderer::Render(void *Data) noexcept
{
  auto *Component = static_cast<ecs::TSkyboxComponent *>(Data);
  ImGui::Text("Path: %s", Component->SkyboxTexture ? Component->SkyboxTexture->GetPath().c_str() : "Not loaded");
}

} // namespace editor

#endif