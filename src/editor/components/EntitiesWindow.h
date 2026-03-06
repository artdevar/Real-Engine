#pragma once

#if DEV_STAGE

#include "EditorWindow.h"
#include "ecs/EntityType.h"
#include <ecs/Core.h>
#include <common/UnorderedVector.h>
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

  const std::optional<ecs::TEntity> &GetSelectedEntity() const
  {
    return m_SelectedEntity;
  }

private:
  void SpawnEntity(ecs::TEntityType _Type);
  int GetSelectedEntityIndex(const CUnorderedVector<ecs::TEntity> &_Entities) const;

  void DisplaySpawnPopup();
  void DisplayEntitiesList();

private:
  IWorldEditor               &m_WorldEditor;
  std::optional<ecs::TEntity> m_SelectedEntity;
  TVector2i                   m_Size;
};

} // namespace editor

#endif