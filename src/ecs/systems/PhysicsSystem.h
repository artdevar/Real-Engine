#pragma once

#include "ecs/SystemManager.h"
#include "interfaces/Updateable.h"

namespace ecs
{

    class CPhysicsSystem : public IUpdateable,
                           public CSystem
    {
    private:
        bool ShouldBeUpdated() const override;
        void UpdateInternal(float _TimeDelta) override;
    };

}
