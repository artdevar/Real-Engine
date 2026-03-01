#pragma once

#include "interfaces/RenderCollector.h"
#include "interfaces/FrameDataCollector.h"
#include "interfaces/Shutdownable.h"
#include "interfaces/Updateable.h"
#include "interfaces/Sharable.h"
#include "interfaces/WorldEditor.h"
#include <common/Common.h>

class CEngine;

namespace ecs
{
class CCoordinator;
} // namespace ecs

class CWorld : public IWorldEditor,
               public IUpdateable,
               public IRenderCollector,
               public IFrameDataCollector,
               public IShutdownable,
               public CSharable<CWorld>
{
  DISABLE_CLASS_COPY(CWorld);

public:
  CWorld();

  ~CWorld();

  void Init();
  void Shutdown() override;

  void Update(float _TimeDelta) override;

  void Collect(TFrameData &_FrameData) override;
  void Collect(CRenderQueue &_Queue) override;

  void DestroyEntity(ecs::TEntity _Entity) override;
  ecs::CEntitySpawner CreateEntitySpawner() override;
  CUnorderedVector<ecs::TEntity> GetEntities() const override;
  CUnorderedVector<ecs::TComponentView> GetEntityComponents(ecs::TEntity _Entity) const override;

protected:
  void InitECS();

public:
  std::unique_ptr<ecs::CCoordinator> m_EntitiesCoordinator;
};