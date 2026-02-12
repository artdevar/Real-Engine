#pragma once

#include "utils/Common.h"
#include "interfaces/Identifiable.h"
#include "interfaces/Updateable.h"
#include "interfaces/Renderable.h"
#include "interfaces/Shutdownable.h"
#include "ecs/CommonECS.h"
#include "utils/UnorderedVector.h"

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

private:
  void UpdateInternal(float _TimeDelta) override;
  bool ShouldBeUpdated() const override;
  void RenderInternal(IRenderer &_Renderer) override;
  bool ShouldBeRendered() const override;

public:
  void RemoveEntity(ecs::TEntity _Entity);

  const CUnorderedVector<ecs::TEntity> &GetAllEntities() const;

  CEntityBuilder CreateEntity();

protected:
  virtual void InitECS();

public:
  std::unique_ptr<CCoordinator> m_EntitiesCoordinator;
};