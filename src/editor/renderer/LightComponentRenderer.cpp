#if DEV_STAGE

#include "pch.h"
#include "LightComponentRenderer.h"
#include "ecs/Components.h"
#include <imgui/imgui.h>

namespace editor
{
ecs::TTypeID TLightComponentRenderer::GetComponentTypeID() noexcept
{
  return ecs::utils::GetComponentTypeID<ecs::TLightComponent>();
}

void TLightComponentRenderer::Render(void *Data) noexcept
{
  auto *Component = static_cast<ecs::TLightComponent *>(Data);

  switch (Component->Type)
  {
  case ELightType::Directional: {
    ImGui::DragFloat3("Direction##LightDir", glm::value_ptr(Component->Direction), 0.1f);
    ImGui::Separator();

    ImGui::ColorEdit3("Color##LightColor", glm::value_ptr(Component->Color),
                      ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::Separator();
    ImGui::DragFloat("Intensity##LightIntensity", &Component->Intensity, 0.1f, 0.0f, 100.0f);
    break;
  }

  default:
    assert(false);
    break;
  }
}

} // namespace editor

#endif