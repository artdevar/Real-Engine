#pragma once

#include "CommonECS.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class CCoordinator final
{
public:
  void Init()
  {
    m_ComponentManager = std::make_unique<CComponentManager>();
    m_EntityManager = std::make_unique<CEntityManager>();
    m_SystemManager = std::make_unique<CSystemManager>();
  }

  ecs::TEntity CreateEntity()
  {
    return m_EntityManager->CreateEntity();
  }

  void DestroyEntity(ecs::TEntity entity)
  {
    m_EntityManager->DestroyEntity(entity);
    m_ComponentManager->EntityDestroyed(entity);
    m_SystemManager->EntityDestroyed(entity);
  }

  const CUnorderedVector<ecs::TEntity> &GetExistingEntities() const
  {
    return m_EntityManager->GetExistingEntities();
  }

  template <typename T>
  void RegisterComponent()
  {
    m_ComponentManager->RegisterComponent<T>();
  }

  template <typename T>
  void AddComponent(ecs::TEntity _Entity, T &&_Component)
  {
    m_ComponentManager->AddComponent(_Entity, std::forward<T>(_Component));

    ecs::TSignature Signature = m_EntityManager->GetSignature(_Entity);
    Signature.set(m_ComponentManager->GetComponentType<T>(), true);
    m_EntityManager->SetSignature(_Entity, Signature);

    m_SystemManager->EntitySignatureChanged(_Entity, Signature);
  }

  template <typename T>
  void RemoveComponent(ecs::TEntity _Entity)
  {
    m_ComponentManager->RemoveComponent<T>(_Entity);

    ecs::TSignature Signature = m_EntityManager->GetSignature(_Entity);
    Signature.set(m_ComponentManager->GetComponentType<T>(), false);
    m_EntityManager->SetSignature(_Entity, Signature);

    m_SystemManager->EntitySignatureChanged(_Entity, Signature);
  }

  template <typename T>
  T &GetComponent(ecs::TEntity _Entity)
  {
    return m_ComponentManager->GetComponent<T>(_Entity);
  }

  template <typename T>
  T *GetComponentSafe(ecs::TEntity _Entity)
  {
    return m_ComponentManager->GetComponentSafe<T>(_Entity);
  }

  template <typename T>
  ecs::TComponentType GetComponentType() const
  {
    return m_ComponentManager->GetComponentType<T>();
  }

  template <typename T>
  bool IsComponentExist(ecs::TEntity _Entity)
  {
    return m_ComponentManager->IsComponentExist<T>(_Entity);
  }

  template <typename T>
  void RegisterSystem()
  {
    m_SystemManager->RegisterSystem<T>()->Init(this);
  }

  template <typename T>
  std::shared_ptr<T> GetSystem() const
  {
    return m_SystemManager->GetSystem<T>();
  }

  template <typename T>
  void SetSystemSignature(const ecs::TSignature &_Signature)
  {
    m_SystemManager->SetSignature<T>(_Signature);
  }

private:
  std::unique_ptr<CComponentManager> m_ComponentManager;
  std::unique_ptr<CEntityManager> m_EntityManager;
  std::unique_ptr<CSystemManager> m_SystemManager;
};