#pragma once

#include "Core.h"

namespace ecs
{

// The one instance of virtual inheritance in the entire implementation.
// An interface is needed so that the ComponentManager (seen later)
// can tell a generic ComponentArray that an entity has been destroyed
// and that it needs to update its array mappings.
class IComponentArray
{
public:
  virtual ~IComponentArray()                    = default;
  virtual void EntityDestroyed(TEntity _Entity) = 0;
};

} // namespace ecs