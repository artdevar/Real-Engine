#pragma once

#if DEV_STAGE
#include "interfaces/Shutdownable.h"
#include "ecs/EntityType.h"
#include <ecs/Core.h>
#include <common/UnorderedVector.h>
#include <optional>

class IWorldEditor;

namespace ecs
{
struct TModelComponent;
struct TTransformComponent;
struct TLightComponent;
} // namespace ecs

class CEditorUI : public IShutdownable
{
public:
  CEditorUI(IWorldEditor &_WorldEditor);
  ~CEditorUI();

  void Shutdown() override;

  void Init();

  void RenderFrame();

private:
  void RenderBegin();
  void RenderEnd();

  void RenderEntities();
  void RenderGlobalParams();

  void SpawnEntity(ecs::TEntityType _Type);

  void RenderEntityData(ecs::TEntity _Entity);
  void RenderEntityData(ecs::TModelComponent &_Mesh);
  void RenderEntityData(ecs::TTransformComponent &_Transform);
  void RenderEntityData(ecs::TLightComponent &_Light);

private:
  int GetSelectedEntityIndex(const CUnorderedVector<ecs::TEntity> &_Entities) const;

private:
  IWorldEditor               &m_WorldEditor;
  std::optional<ecs::TEntity> m_SelectedEntity;
};

#endif