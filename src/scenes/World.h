#pragma once

#include "interfaces/RenderCollector.h"
#include "interfaces/FrameDataCollector.h"
#include "interfaces/WorldEditor.h"
#include <common/interfaces/Shutdownable.h>
#include <common/interfaces/Updateable.h>
#include <common/Sharable.h>
#include <events/EventsListener.h>

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
               public IEventsListener,
               public CSharable<CWorld>
{
  DISABLE_CLASS_COPY(CWorld);

public:
  CWorld();

  ~CWorld();

  void Init();
  void Shutdown() override;

  void OnEvent(const TEvent &_Event) override;

  void Update(float _TimeDelta) override;

  void Collect(TFrameData &_FrameData) override;
  void Collect(CRenderQueue &_Queue) override;

  std::string GetEntityName(ecs::TEntity _Entity) const override;
  ecs::TEntity CloneEntity(ecs::TEntity _Entity) override;
  void DestroyEntity(ecs::TEntity _Entity) override;
  ecs::CEntitySpawner CreateEntitySpawner() override;
  CUnorderedVector<ecs::TEntity> GetEntities() const override;
  CUnorderedVector<ecs::TComponentView> GetEntityComponents(ecs::TEntity _Entity) const override;

protected:
  void InitECS();
  void SubscribeToEvents();

public:
  std::unique_ptr<ecs::CCoordinator> m_EntitiesCoordinator;
};