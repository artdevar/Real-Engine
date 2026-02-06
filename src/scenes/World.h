#pragma once

#include "utils/Common.h"
#include "interfaces/Identifiable.h"
#include "interfaces/Updateable.h"
#include "interfaces/Renderable.h"
#include "interfaces/Shutdownable.h"
#include "ecs/CommonECS.h"
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>

class CEngine;
class CCoordinator;
class CEntityBuilder;

class CWorld : public IIdentifiable,
               public IUpdateable,
               public IRenderable,
               public IShutdownable
{
  DISABLE_CLASS_COPY(CWorld);

public:
  CWorld();

  ~CWorld();

  void Init();
  void Shutdown() override;

  void Update(float _TimeDelta) override;
  void Render(CRenderer &_Renderer) override;

public:
  void RemoveEntity(ecs::TEntity _Entity);

  const std::vector<ecs::TEntity> &GetAllEntities() const;

  CEntityBuilder GetEntityBuilder() const;

protected:
  virtual void InitECS();

public:
  std::unique_ptr<CCoordinator> m_EntitiesCoordinator;
};