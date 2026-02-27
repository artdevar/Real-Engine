#pragma once

#include <ecs/IEntitiesBroker.h>

class IWorldEditor : public ecs::IEntitiesBroker
{
public:
  virtual ~IWorldEditor() = default;
};