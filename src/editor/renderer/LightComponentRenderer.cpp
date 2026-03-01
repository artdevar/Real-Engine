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
    ImGui::DragFloat3("Direction##LightDir", glm::value_ptr(Component->Direction), 0.25f);
    ImGui::Separator();

    ImGui::ColorEdit3("Ambient color##LightAmbColor", glm::value_ptr(Component->Ambient),
                      ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::Separator();

    ImGui::ColorEdit3("Diffuse color##LightDiffColor", glm::value_ptr(Component->Diffuse),
                      ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::Separator();

    ImGui::ColorEdit3("Specular color##LightSpecColor", glm::value_ptr(Component->Specular),
                      ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    break;
  }

  case ELightType::Point: {
    ImGui::DragFloat3("Position##LightPos", glm::value_ptr(Component->Position), 0.25f);
    ImGui::Separator();

    ImGui::ColorEdit3("Ambient color##LightAmbColor", glm::value_ptr(Component->Ambient), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Diffuse color##LightDiffColor", glm::value_ptr(Component->Diffuse), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Specular color##LightSpecColor", glm::value_ptr(Component->Specular),
                      ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::DragFloat("LinearDrag", &Component->Linear, 0.005f, 0.0f, 5.0f);
    ImGui::DragFloat("Quadratic", &Component->Quadratic, 0.005f, 0.0f, 5.0f);
    break;
  }

  case ELightType::Spotlight: {
    ImGui::DragFloat3("Position##LightPos", glm::value_ptr(Component->Position), 0.25f);
    ImGui::Separator();

    ImGui::DragFloat3("Direction##LightDir", glm::value_ptr(Component->Direction), 0.25f);
    ImGui::Separator();

    ImGui::ColorEdit3("Ambient color##LightAmbColor", glm::value_ptr(Component->Ambient), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Diffuse color##LightDiffColor", glm::value_ptr(Component->Diffuse), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Specular color##LightSpecColor", glm::value_ptr(Component->Specular),
                      ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    break;
  }

  default:
    assert(false);
    break;
  }
}

} // namespace editor

#endif