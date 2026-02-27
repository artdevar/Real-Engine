#pragma once

#include "Core.h"
#include <common/UnorderedVector.h>
#include <memory>

namespace ecs
{

class CComponentManager;
class CEntityManager;
class CSystemManager;

class CCoordinator final
{
public:
  void Init();

  TEntity CreateEntity();
  void DestroyEntity(TEntity _Entity);

  CUnorderedVector<TEntity> GetEntities() const;

  template <typename T>
  void RegisterComponent();

  template <typename T>
  void AddComponent(TEntity _Entity, T &&_Component);

  template <typename T>
  void RemoveComponent(TEntity _Entity);

  template <typename T>
  T &GetComponent(TEntity _Entity);

  template <typename T>
  TComponentType GetComponentType() const;

  template <typename T>
  bool DoesComponentExist(TEntity _Entity);

  template <typename T>
  void RegisterSystem();

  template <typename T>
  std::shared_ptr<T> GetSystem() const;

  template <typename T>
  void SetSystemSignature(const TSignature &_Signature);

private:
  std::unique_ptr<CComponentManager> m_ComponentManager;
  std::unique_ptr<CEntityManager>    m_EntityManager;
  std::unique_ptr<CSystemManager>    m_SystemManager;
};

} // namespace ecs

#include "impl/CoordinatorImpl.h"