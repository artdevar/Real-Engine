#if DEV_STAGE

#include "EntitiesWindow.h"

#include "../platform/SysUtils.h"
#include "utils/Resource.h"
#include "ecs/Components.h"
#include "ecs/ComponentsFactory.h"
#include <ecs/EntityType.h>
#include <ecs/EntitySpawner.h>
#include "interfaces/WorldEditor.h"
#include <imgui/imgui.h>
#include <algorithm>
#include <glm/mat4x4.hpp>
#include <cassert>
#include <filesystem>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace editor
{

static constexpr std::pair<const char *, ecs::TEntityType> ENTITY_TYPES[] = {std::make_pair("Static mesh", ecs::TEntityType::StaticMesh),
                                                                             std::make_pair("Directional light", ecs::TEntityType::DirectionalLight),
                                                                             std::make_pair("Skybox", ecs::TEntityType::Skybox)};

CEntitiesWindow::CEntitiesWindow(IWorldEditor &_WorldEditor) :
    m_WorldEditor(_WorldEditor)
{
}

void CEntitiesWindow::Render()
{
  if (ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
  {
    DisplaySpawnPopup();
    DisplayEntitiesList();

    const ImVec2 Available = ImGui::GetContentRegionAvail();
    m_Size                 = TVector2i(static_cast<int>(Available.x), static_cast<int>(Available.y));
  }

  ImGui::End();
}

void CEntitiesWindow::DisplaySpawnPopup()
{
  static bool SpawnPopupOpen = false;
  if (ImGui::Button("Add##AddEntity"))
  {
    ImGui::OpenPopup("Spawn entity");
    SpawnPopupOpen = true;
  }

  if (m_SelectedEntity.has_value())
  {
    ImGui::SameLine();
    if (ImGui::Button("Dublicate##DupEntity"))
    {
      //m_SelectedEntity = m_WorldEditor.Clone(m_SelectedEntity.value());
    }

    ImGui::SameLine();
    if (ImGui::Button("Delete##DelEntity"))
    {
      m_WorldEditor.DestroyEntity(m_SelectedEntity.value());
      m_SelectedEntity.reset();
    }
  }

  if (ImGui::BeginPopupModal("Spawn entity", &SpawnPopupOpen, ImGuiWindowFlags_AlwaysAutoResize))
  {
    static int CurrentIndex = -1;

    if (ImGui::BeginListBox("##EntityTypeList"))
    {
      for (int i = 0; i < IM_ARRAYSIZE(ENTITY_TYPES); ++i)
      {
        const bool IsSelected = (CurrentIndex == i);
        if (ImGui::Selectable(ENTITY_TYPES[i].first, IsSelected))
          CurrentIndex = i;

        if (IsSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndListBox();
    }

    if (CurrentIndex != -1)
    {
      if (ImGui::Button("Spawn"))
      {
        SpawnEntity(ENTITY_TYPES[CurrentIndex].second);
        ImGui::CloseCurrentPopup();
      }
    }

    if (!SpawnPopupOpen)
      CurrentIndex = -1;

    ImGui::EndPopup();
  }
}

void CEntitiesWindow::DisplayEntitiesList()
{
  const CUnorderedVector<ecs::TEntity> &Entities = m_WorldEditor.GetEntities();

  std::vector<std::string> EntitiesNames;
  EntitiesNames.reserve(Entities.Size());

  for (auto Entity : Entities)
    EntitiesNames.push_back(std::to_string(Entity));

  int CurrentEntityIndex = GetSelectedEntityIndex(Entities);

  ImGuiStyle  &Style    = ImGui::GetStyle();
  const ImVec4 WindowBg = Style.Colors[ImGuiCol_WindowBg];

  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, WindowBg);
  ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, WindowBg);
  ImGui::PushStyleColor(ImGuiCol_FrameBgActive, WindowBg);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

  if (ImGui::ListBox(
          "##EntitiesList", &CurrentEntityIndex,
          [](void *_Vec, int _Index, const char **_Ret) -> bool {
            auto EntitiesNames = reinterpret_cast<std::vector<std::string> *>(_Vec);

            if (_Index < 0 || _Index >= static_cast<int>(EntitiesNames->size()))
              return false;

            *_Ret = EntitiesNames->at(_Index).c_str();
            return true;
          },
          reinterpret_cast<void *>(&EntitiesNames), static_cast<int>(EntitiesNames.size()), 8))
  {
    m_SelectedEntity = Entities[CurrentEntityIndex];
  }

  ImGui::PopStyleVar();
  ImGui::PopStyleColor(3);
}

void CEntitiesWindow::SpawnEntity(ecs::TEntityType _Type)
{
  switch (_Type)
  {
  case ecs::TEntityType::StaticMesh: {
    const std::filesystem::path PathToLoad = utils::OpenFileDialog(utils::EFileDialogMode::SelectFile);
    if (PathToLoad.empty())
      return;

    std::shared_ptr<CModel> Model = resource::LoadModel(PathToLoad);
    if (!Model)
      return;

    auto &&TransformComponent = ecs::CComponentsFactory::Create<ecs::TTransformComponent>(glm::mat4x4(1.0f));
    auto &&ModelComponent     = ecs::CComponentsFactory::Create<ecs::TModelComponent>(Model);

    ecs::CEntitySpawner Builder = m_WorldEditor.CreateEntitySpawner();
    Builder.AddComponent(std::move(TransformComponent)).AddComponent(std::move(ModelComponent));
    m_SelectedEntity = Builder.Spawn();

    break;
  }

  case ecs::TEntityType::Skybox: {
    const std::filesystem::path PathToLoad = utils::OpenFileDialog(utils::EFileDialogMode::SelectFolder);
    if (PathToLoad.empty())
      return;

    std::shared_ptr<CTextureBase> Cubemap = resource::LoadCubemap(PathToLoad);
    if (!Cubemap)
      return;

    auto &&SkyboxComponent = ecs::CComponentsFactory::Create<ecs::TSkyboxComponent>(Cubemap);
    m_SelectedEntity       = m_WorldEditor.CreateEntitySpawner().AddComponent(std::move(SkyboxComponent)).Spawn();

    break;
  }

  case ecs::TEntityType::PointLight: {
    auto &&LightComponent = ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Point);
    m_SelectedEntity      = m_WorldEditor.CreateEntitySpawner().AddComponent(std::move(LightComponent)).Spawn();

    break;
  }

  case ecs::TEntityType::DirectionalLight: {
    auto &&LightComponent = ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Directional);
    m_SelectedEntity      = m_WorldEditor.CreateEntitySpawner().AddComponent(std::move(LightComponent)).Spawn();

    break;
  }

  case ecs::TEntityType::Spotlight: {
    auto &&LightComponent = ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Spotlight);
    m_SelectedEntity      = m_WorldEditor.CreateEntitySpawner().AddComponent(std::move(LightComponent)).Spawn();

    break;
  }

  default:
    assert(false);
    break;
  }
}

int CEntitiesWindow::GetSelectedEntityIndex(const CUnorderedVector<ecs::TEntity> &_Entities) const
{
  if (m_SelectedEntity.has_value())
  {
    const auto Iterator = _Entities.Find(m_SelectedEntity.value());
    if (Iterator == _Entities.end())
      return -1;

    return static_cast<int>(std::distance(_Entities.begin(), Iterator));
  }

  return -1;
}

} // namespace editor

#endif
