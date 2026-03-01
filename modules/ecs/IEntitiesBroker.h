#pragma once

#include "Core.h"
#include <common/UnorderedVector.h>

namespace ecs
{
class CEntitySpawner;
struct TComponentView;

class IEntitiesBroker
{
public:
  virtual ~IEntitiesBroker() = default;

  virtual void DestroyEntity(TEntity _Entity)                                         = 0;
  virtual CEntitySpawner CreateEntitySpawner()                                        = 0;
  virtual CUnorderedVector<TEntity> GetEntities() const                               = 0;
  virtual CUnorderedVector<TComponentView> GetEntityComponents(TEntity _Entity) const = 0;
};

} // namespace ecs