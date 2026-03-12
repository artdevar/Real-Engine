#if DEV_STAGE

#include "ModelComponentRenderer.h"
#include "assets/Texture.h"
#include "ecs/Components.h"
#include <imgui/imgui.h>

namespace editor
{

static void DisplayTexture(const ecs::TModelComponent::TTexture &Texture)
{
  const auto   AvailableRegion = ImGui::GetContentRegionAvail();
  const ImVec2 ImageSize(AvailableRegion.x, AvailableRegion.x);
  if (Texture.Texture)
    ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(Texture.Texture->ID())), ImageSize);
  else
    ImGui::Text("No texture");
}

ecs::TTypeID TModelComponentRenderer::GetComponentTypeID() noexcept
{
  return ecs::utils::GetComponentTypeID<ecs::TModelComponent>();
}

void TModelComponentRenderer::Render(void *Data) noexcept
{
  auto *Component = static_cast<ecs::TModelComponent *>(Data);

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
  ImGui::Separator();

  for (size_t i = 0; i < Component->Materials.size(); ++i)
  {
    auto &Material = Component->Materials[i];

    ImGui::PushID(static_cast<int>(i));
    const std::string Header = std::format("Material {}", i + 1);
    if (ImGui::CollapsingHeader(Header.c_str()))
    {
      if (ImGui::CollapsingHeader("Base Color Texture"))
      {
        DisplayTexture(Material.BaseColorTexture);
        ImGui::Text("Base color factor: (%.2f, %.2f, %.2f, %.2f)", Material.BaseColorFactor.r, Material.BaseColorFactor.g, Material.BaseColorFactor.b,
                    Material.BaseColorFactor.a);
      }

      if (ImGui::CollapsingHeader("Metallic Roughness Texture"))
      {
        DisplayTexture(Material.MetallicRoughnessTexture);
        ImGui::Text("Metallic factor: %.2f", Material.MetallicFactor);
        ImGui::Text("Roughness factor: %.2f", Material.RoughnessFactor);
      }

      if (ImGui::CollapsingHeader("Normal Texture"))
      {
        DisplayTexture(Material.NormalTexture);
      }

      if (ImGui::CollapsingHeader("Occlusion Texture"))
      {
        DisplayTexture(Material.OcclusionTexture);
        ImGui::Text("Occlusion strength: %.2f", Material.OcclusionStrength);
      }

      if (ImGui::CollapsingHeader("Emissive Texture"))
      {
        DisplayTexture(Material.EmissiveTexture);
        ImGui::Text("Emissive factor: (%.2f, %.2f, %.2f)", Material.EmissiveFactor.r, Material.EmissiveFactor.g, Material.EmissiveFactor.b);
      }

      ImGui::Separator();

      ImGui::Text("Alpha mode: %s", Material.AlphaMode == EAlphaMode::Opaque ? "Opaque" : Material.AlphaMode == EAlphaMode::Mask ? "Mask" : "Blend");
      ImGui::Text("Is double sided: %s", Material.IsDoubleSided ? "Yes" : "No");
    }
    ImGui::PopID();
  }
}

} // namespace editor

#endif