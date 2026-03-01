#pragma once

#include "Core.h"
#include <string>

namespace ecs
{

class IComponentArray
{
public:
  virtual ~IComponentArray()                     = default;
  virtual void EntityDestroyed(TEntity _Entity)  = 0;
  virtual void *GetRawComponent(TEntity _Entity) = 0;
  virtual TTypeID GetComponentTypeID() const     = 0;
  virtual TTypeName GetComponentName() const     = 0;
};

} // namespace ecs