#include "ecs/systems/PhysicsSystem.h"

namespace ecs
{

void CPhysicsSystem::UpdateInternal(float _TimeDelta)
{
}

bool CPhysicsSystem::ShouldBeUpdated() const
{
  return !m_Entities.Empty();
}

} // namespace ecs
