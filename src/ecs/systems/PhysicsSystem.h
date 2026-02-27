#pragma once

#include "interfaces/Updateable.h"
#include <ecs/System.h>

namespace ecs
{

class CPhysicsSystem : public IUpdateable,
                       public CSystem
{
private:
  bool ShouldBeUpdated() const override;
  void UpdateInternal(float _TimeDelta) override;
};

} // namespace ecs
