#include "Coordinator.h"

namespace ecs
{

void CCoordinator::Init()
{
  m_ComponentManager = std::make_unique<CComponentManager>();
  m_EntityManager    = std::make_unique<CEntityManager>();
  m_SystemManager    = std::make_unique<CSystemManager>();
}

ecs::TEntity CCoordinator::CreateEntity()
{
  return m_EntityManager->CreateEntity();
}

void CCoordinator::DestroyEntity(ecs::TEntity _Entity)
{
  m_EntityManager->DestroyEntity(_Entity);
  m_ComponentManager->EntityDestroyed(_Entity);
  m_SystemManager->EntityDestroyed(_Entity);
}

CUnorderedVector<TEntity> CCoordinator::GetEntities() const
{
  return m_EntityManager->GetAliveEntities();
}

} // namespace ecs