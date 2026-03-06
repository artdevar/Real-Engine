#pragma once

#include <ecs/IEntitiesBroker.h>

class IWorldEditor : public ecs::IEntitiesBroker
{
public:
  virtual ~IWorldEditor() = default;

  virtual std::string GetEntityName(ecs::TEntity _Entity) const = 0;
};