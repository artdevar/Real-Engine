#include "EntityManager.h"
#include <algorithm>
#include <cassert>

CEntityManager::CEntityManager()
{
  for (ecs::TEntity Entity = 0; Entity < ecs::MAX_ENTITIES; ++Entity)
    m_AvailableEntities.push(Entity);
}

ecs::TEntity CEntityManager::CreateEntity()
{
  assert(m_ExistingEntities.size() < ecs::MAX_ENTITIES && "Too many entities in existence.");

  ecs::TEntity ID = m_AvailableEntities.front();
  m_AvailableEntities.pop();

  m_ExistingEntities.push_back(ID);

  return ID;
}

void CEntityManager::DestroyEntity(ecs::TEntity _Entity)
{
  assert(_Entity < ecs::MAX_ENTITIES && "Entity out of range.");
  assert(std::find(m_ExistingEntities.begin(), m_ExistingEntities.end(), _Entity) != m_ExistingEntities.end());

  m_Signatures[_Entity].reset();
  m_AvailableEntities.push(_Entity);
  m_ExistingEntities.erase(std::find(m_ExistingEntities.begin(), m_ExistingEntities.end(), _Entity));
}

const std::vector<ecs::TEntity> & CEntityManager::GetExistingEntities() const
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