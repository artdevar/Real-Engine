#include "ecs/systems/PhysicsSystem.h"

namespace ecs
{

    void CPhysicsSystem::UpdateInternal(float _TimeDelta)
    {
    }

    bool CPhysicsSystem::ShouldBeUpdated() const
    {
        return IUpdateable::ShouldBeUpdated() && !m_Entities.Empty();
    }

}
