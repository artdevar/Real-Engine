#pragma once

#include "ecs/CommonECS.h"
#include "interfaces/Identifiable.h"
#include "interfaces/RenderCollector.h"
#include "interfaces/FrameDataCollector.h"
#include "interfaces/Shutdownable.h"
#include "interfaces/Updateable.h"
#include "utils/Common.h"
#include "utils/UnorderedVector.h"

class CEngine;
class CCoordinator;
class CEntityBuilder;

class CWorld : public IIdentifiable,
               public IUpdateable,
               public IRenderCollector,
               public IFrameDataCollector,
               public IShutdownable
{
  DISABLE_CLASS_COPY(CWorld);

public:
  CWorld();

  ~CWorld();

  void Init();
  void Shutdown() override;

  void Collect(TFrameData &_FrameData) override;
  void Collect(CRenderQueue &_Queue) override;

private:
  void UpdateInternal(float _TimeDelta) override;
  bool ShouldBeUpdated() const override;

public:
  void RemoveEntity(ecs::TEntity _Entity);

  const CUnorderedVector<ecs::TEntity> &GetAllEntities() const;

  CEntityBuilder CreateEntity();

protected:
  void InitECS();

public:
  std::unique_ptr<CCoordinator> m_EntitiesCoordinator;
};