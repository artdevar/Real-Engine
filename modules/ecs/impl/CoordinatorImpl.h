#pragma once

#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"

namespace ecs
{

template <typename T>
void CCoordinator::RegisterComponent()
{
  m_ComponentManager->RegisterComponent<T>();
}

template <typename T>
void CCoordinator::AddComponent(TEntity _Entity, T &&_Component)
{
  m_ComponentManager->AddComponent(_Entity, std::forward<T>(_Component));

  ecs::TSignature Signature = m_EntityManager->GetSignature(_Entity);
  Signature.set(m_ComponentManager->GetComponentType<T>(), true);
  m_EntityManager->SetSignature(_Entity, Signature);

  m_SystemManager->EntitySignatureChanged(_Entity, Signature);
}

template <typename T>
void CCoordinator::RemoveComponent(TEntity _Entity)
{
  m_ComponentManager->RemoveComponent<T>(_Entity);

  ecs::TSignature Signature = m_EntityManager->GetSignature(_Entity);
  Signature.set(m_ComponentManager->GetComponentType<T>(), false);
  m_EntityManager->SetSignature(_Entity, Signature);

  m_SystemManager->EntitySignatureChanged(_Entity, Signature);
}

template <typename T>
T &CCoordinator::GetComponent(TEntity _Entity)
{
  return m_ComponentManager->GetComponent<T>(_Entity);
}

template <typename T>
TComponentType CCoordinator::GetComponentType() const
{
  return m_ComponentManager->GetComponentType<T>();
}

template <typename T>
bool CCoordinator::DoesComponentExist(TEntity _Entity)
{
  return m_ComponentManager->DoesComponentExist<T>(_Entity);
}

template <typename T>
void CCoordinator::RegisterSystem()
{
  m_SystemManager->RegisterSystem<T>()->Init(this);
}

template <typename T>
std::shared_ptr<T> CCoordinator::GetSystem() const
{
  return m_SystemManager->GetSystem<T>();
}

template <typename T>
void CCoordinator::SetSystemSignature(const TSignature &_Signature)
{
  m_SystemManager->SetSignature<T>(_Signature);
}

} // namespace ecs