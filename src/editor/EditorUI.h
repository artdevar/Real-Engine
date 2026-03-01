#pragma once

#if DEV_STAGE
#include "renderer/ComponentRenderer.h"
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

namespace editor
{

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
  void RenderEntity(ecs::TEntity _Entity);
  void RenderGlobalParams();

  void SpawnEntity(ecs::TEntityType _Type);

private:
  int GetSelectedEntityIndex(const CUnorderedVector<ecs::TEntity> &_Entities) const;

private:
  IWorldEditor               &m_WorldEditor;
  CComponentRenderer          m_ComponentRenderer;
  std::optional<ecs::TEntity> m_SelectedEntity;
};

} // namespace editor
#endif