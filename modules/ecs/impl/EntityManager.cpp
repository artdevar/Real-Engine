#include "EntityManager.h"
#include <cassert>

namespace ecs
{

CEntityManager::CEntityManager() :
    m_Signatures(ecs::TSignature{})
{
  for (int Entity = ecs::MAX_ENTITIES - 1; Entity >= 0; --Entity)
    m_AvailableEntities.PushBack(static_cast<ecs::TEntity>(Entity));
}

ecs::TEntity CEntityManager::CreateEntity()
{
  ecs::TEntity ID = m_AvailableEntities.Back();
  m_AvailableEntities.PopBack();
  m_AliveEntities.Push(ID);

  return ID;
}

void CEntityManager::DestroyEntity(ecs::TEntity _Entity)
{
  assert(IsEntityExist(_Entity) && "Destroying non-existent entity.");

  m_Signatures[_Entity].reset();
  m_AvailableEntities.PushBack(_Entity);
  m_AliveEntities.Erase(_Entity);
}

void CEntityManager::SetSignature(ecs::TEntity _Entity, ecs::TSignature _Signature)
{
  assert(_Entity < ecs::MAX_ENTITIES && "Entity out of range.");
  m_Signatures[_Entity] = std::move(_Signature);
}

const ecs::TSignature &CEntityManager::GetSignature(ecs::TEntity _Entity) const
{
  assert(_Entity < ecs::MAX_ENTITIES && "Entity out of range.");
  return m_Signatures[_Entity];
}

bool CEntityManager::IsEntityExist(ecs::TEntity _Entity) const
{
  return _Entity < ecs::MAX_ENTITIES && !m_AvailableEntities.Contains(_Entity);
}

const CUnorderedVector<ecs::TEntity> &CEntityManager::GetAliveEntities() const
{
  return m_AliveEntities;
}

} // namespace ecs