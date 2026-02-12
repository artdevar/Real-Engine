#pragma once

#if DEV_STAGE
#include "interfaces/Shutdownable.h"
#include "interfaces/Updateable.h"
#include "interfaces/Renderable.h"
#include "ecs/CommonECS.h"
#include "ecs/Components.h"
#include "utils/UnorderedVector.h"

class CEngine;
class CObject;
class CShader;

class CEditorUI : public IUpdateable,
                  public IRenderable,
                  public IShutdownable
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

  void Init(CEngine *_Engine);

private:
  void UpdateInternal(float _TimeDelta) override;
  bool ShouldBeUpdated() const override;
  void RenderInternal(CRenderer &_Renderer) override;
  bool ShouldBeRendered() const override;

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
  CEngine *m_Engine;

  std::optional<ecs::TEntity> m_SelectedEntity;
};

#endif