#if DEV_STAGE

#include "SkyboxComponentRenderer.h"
#include "ecs/Components.h"
#include "assets/Texture.h"
#include <imgui/imgui.h>
#include "utils/Path.h"

namespace editor
{
static void DisplayTextureInfo(const std::shared_ptr<CTexture> &Texture)
{
  if (Texture)
  {
    const auto TexturePath = Texture->GetPath();
    const auto TextureSize = Texture->GetSize();
    ImGui::Text("Path: %s", utils::GetRelativePath(TexturePath).string().c_str());
    ImGui::Text("Resolution: %d x %d", TextureSize.X, TextureSize.Y);
  }
  else
  {
    ImGui::Text("No texture assigned");
  }
}

ecs::TTypeID TSkyboxComponentRenderer::GetComponentTypeID() noexcept
{
  return ecs::utils::GetComponentTypeID<ecs::TEnvironmentComponent>();
}

void TSkyboxComponentRenderer::Render(void *Data) noexcept
{
  auto *Component = static_cast<ecs::TEnvironmentComponent *>(Data);

  if (ImGui::CollapsingHeader("Skybox"))
  {
    ImGui::Indent();
    DisplayTextureInfo(Component->SkyboxTexture);
    ImGui::Unindent();
  }

  if (ImGui::CollapsingHeader("Equirectangular Map"))
  {
    ImGui::Indent();
    DisplayTextureInfo(Component->EquirectangularMap);
    ImGui::Unindent();
  }

  if (ImGui::CollapsingHeader("Irradiance Map"))
  {
    ImGui::Indent();
    DisplayTextureInfo(Component->IrradianceMap);
    ImGui::Unindent();
  }
}

} // namespace editor

#endif