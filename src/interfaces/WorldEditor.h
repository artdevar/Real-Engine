#pragma once

#include <ecs/IEntitiesBroker.h>

namespace ecs
{
struct TNameComponent;
struct TTransformComponent;
} // namespace ecs

class IWorldEditor : public ecs::IEntitiesBroker
{
public:
  virtual ~IWorldEditor() = default;

  virtual ecs::TNameComponent *GetEntityName(ecs::TEntity _Entity) const     = 0;
  virtual ecs::TTransformComponent *GetTransform(ecs::TEntity _Entity) const = 0;
};