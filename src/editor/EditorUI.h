#pragma once

#if DEV_STAGE
#include "ecs/CommonECS.h"
#include "ecs/Components.h"
#include "interfaces/Shutdownable.h"
#include "utils/UnorderedVector.h"

class CEditorUI : public IShutdownable
{
  enum TEntityType
  {
    StaticMesh,
    PointLight,
    DirectionalLight,
    Spotlight,
    Skybox
  };

public:
  void Shutdown() override;

  void Init();

  void RenderFrame();

private:
  void RenderBegin();
  void RenderEnd();

  void RenderEntities();
  void RenderGlobalParams();
  void RenderLightDebugLines();

  void SpawnEntity(TEntityType _Type);

  void RenderEntityData(ecs::TEntity _Entity);
  void RenderEntityData(ecs::TModelComponent &_Mesh);
  void RenderEntityData(ecs::TTransformComponent &_Transform);
  void RenderEntityData(ecs::TLightComponent &_Light);

private:
  int GetSelectedEntityIndex(const CUnorderedVector<ecs::TEntity> &_Entities) const;

private:
  std::optional<ecs::TEntity> m_SelectedEntity;
};

#endif