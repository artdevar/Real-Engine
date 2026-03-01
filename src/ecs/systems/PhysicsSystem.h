#pragma once

#include "interfaces/Updateable.h"
#include <ecs/System.h>

namespace ecs
{

class CPhysicsSystem : public IUpdateable,
                       public CSystem
{
public:
  void Update(float _TimeDelta) override;
};

} // namespace ecs
