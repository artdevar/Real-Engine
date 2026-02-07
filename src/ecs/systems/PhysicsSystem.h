#pragma once

#include "ecs/SystemManager.h"
#include "interfaces/Updateable.h"

namespace ecs
{

    class CPhysicsSystem : public IUpdateable,
                           public CSystem
    {
    public:
        void Update(float _TimeDelta) override;
    };

}
