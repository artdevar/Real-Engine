#pragma once

#if DEV_STAGE

#include "EditorWindow.h"
#include "ecs/EntityType.h"
#include <ecs/Core.h>
#include <common/containers/UnorderedVector.h>
#include <imgui/imgui.h>
#include <optional>

class IWorldEditor;

namespace editor
{

class CEntitiesWindow : public IEditorWindow
{
public:
  CEntitiesWindow(IWorldEditor &_WorldEditor);

  void Render();

  std::string GetName() const override
  {
    return "Entities";
  }

  TVector2i GetSize() const override
  {
    return m_Size;
  }

  const std::optional<ecs::TEntity> &GetSelectedEntity() const;

private:
  void SpawnEntity(ecs::TEntityType _Type);
  int GetSelectedEntityIndex(const CUnorderedVector<ecs::TEntity> &_Entities) const;

  void DisplaySpawnPopup();
  void DisplayEntitiesList();

  void SelectEntity(ecs::TEntity _Entity);
  void DeselectEntity();

private:
  IWorldEditor               &m_WorldEditor;
  std::optional<ecs::TEntity> m_SelectedEntity;
  TVector2i                   m_Size;
};

} // namespace editor

#endif