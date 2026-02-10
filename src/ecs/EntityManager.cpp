#include "EntityManager.h"
#include <algorithm>
#include <cassert>

CEntityManager::CEntityManager()
{
  for (int Entity = ecs::MAX_ENTITIES - 1; Entity >= 0; --Entity)
    m_AvailableEntities.PushBack(static_cast<ecs::TEntity>(Entity));
}

ecs::TEntity CEntityManager::CreateEntity()
{
  assert(m_ExistingEntities.Size() < ecs::MAX_ENTITIES && "Too many entities in existence");

  ecs::TEntity ID = m_AvailableEntities.Back();
  m_AvailableEntities.PopBack();

  m_ExistingEntities.Push(ID);

  return ID;
}

void CEntityManager::DestroyEntity(ecs::TEntity _Entity)
{
  assert(_Entity < ecs::MAX_ENTITIES && "Entity out of range");

  m_Signatures[_Entity].reset();
  m_AvailableEntities.PushBack(_Entity);
  m_ExistingEntities.Erase(_Entity);
}

const CUnorderedVector<ecs::TEntity> &CEntityManager::GetExistingEntities() const
{
  return m_ExistingEntities;
}

void CEntityManager::SetSignature(ecs::TEntity _Entity, ecs::TSignature _Signature)
{
  assert(_Entity < ecs::MAX_ENTITIES && "Entity out of range.");
  m_Signatures[_Entity] = _Signature;
}

ecs::TSignature CEntityManager::GetSignature(ecs::TEntity _Entity) const
{
  assert(_Entity < ecs::MAX_ENTITIES && "Entity out of range.");
  return m_Signatures[_Entity];
}