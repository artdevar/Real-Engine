#pragma once

#include "interfaces/Shutdownable.h"
#include "interfaces/Updateable.h"
#include "interfaces/Renderable.h"
#include "ecs/CommonECS.h"
#include "ecs/Components.h"
#include "Shared.h"
#include <glm/mat4x4.hpp>
#include <optional>
#include <memory>

class CEngine;
class CObject;
class CShader;

class CEditorUI :
  public IUpdateable,
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

  void Init(CEngine * _Game);

  void Update(float _TimeDelta) override;

  void Render(CRenderer & _Renderer) override;

private:

  void RenderBegin();

  void RenderEnd();

  void RenderEntities();

  void SpawnEntity(TEntityType _Type);

  void RenderEntityData(ecs::TEntity _Entity);

  void RenderEntityData(ecs::TModelComponent & _Mesh);

  void RenderEntityData(ecs::TTransformComponent & _Transform);

  void RenderEntityData(ecs::TLightComponent & _Light);

private:

  CEngine * m_Engine;

  std::optional<ecs::TEntity> m_CurrentEntity;
  int                         m_LightIndex = -1;

};