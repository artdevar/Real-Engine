#pragma once

#include "CommonECS.h"
#include "utils/UnorderedVector.h"
#include <cassert>
#include <ctti/type_id.hpp>
#include <memory>
#include <set>
#include <unordered_map>

class CCoordinator;

class CSystem
{
public:
  virtual void Init(CCoordinator *_Coordinator)
  {
    m_Coordinator = _Coordinator;
  }

  void AddEntity(ecs::TEntity _Entity)
  {
    if (m_Entities.PushUnique(_Entity))
      OnEntityAdded(_Entity);
  }

  void DeleteEntity(ecs::TEntity _Entity)
  {
    if (m_Entities.SafeErase(_Entity))
      OnEntityDeleted(_Entity);
  }

protected:
  virtual void OnEntityAdded(ecs::TEntity _Entity)
  {
    // Empty
  }

  virtual void OnEntityDeleted(ecs::TEntity _Entity)
  {
    // Empty
  }

protected:
  CCoordinator                  *m_Coordinator;
  CUnorderedVector<ecs::TEntity> m_Entities;
};

class CSystemManager
{
public:
  template <typename T>
  std::shared_ptr<T> RegisterSystem()
  {
    static_assert(std::is_base_of_v<CSystem, T>);

    const ctti::type_id_t TypeID = ctti::type_id<T>();
    assert(m_Systems.find(TypeID) == m_Systems.end() && "Registering system more than once.");

    std::shared_ptr<CSystem> System = std::make_shared<T>();
    m_Systems.emplace(TypeID, System);
    return std::static_pointer_cast<T>(System);
  }

  template <typename T>
  std::shared_ptr<T> GetSystem() const
  {
    static_assert(std::is_base_of_v<CSystem, T>);

    const ctti::type_id_t    TypeID = ctti::type_id<T>();
    std::shared_ptr<CSystem> System = m_Systems.at(TypeID);
    return std::static_pointer_cast<T>(System);
  }

  template <typename T>
  void SetSignature(const ecs::TSignature &_Signature)
  {
    const ctti::type_id_t TypeID = ctti::type_id<T>();
    assert(m_Systems.find(TypeID) != m_Systems.end() && "System used before registered.");

    m_Signatures.emplace(TypeID, _Signature);
  }

  void EntityDestroyed(ecs::TEntity _Entity)
  {
    for (const auto &[Name, System] : m_Systems)
      System->DeleteEntity(_Entity);
  }

  void EntitySignatureChanged(ecs::TEntity _Entity, const ecs::TSignature &_EntitySignature)
  {
    for (const auto &[Name, System] : m_Systems)
    {
      const ecs::TSignature &SystemSignature = m_Signatures[Name];

      if ((_EntitySignature & SystemSignature) == SystemSignature)
        System->AddEntity(_Entity);
      else
        System->DeleteEntity(_Entity);
    }
  }

private:
  std::unordered_map<ctti::type_id_t, ecs::TSignature>          m_Signatures;
  std::unordered_map<ctti::type_id_t, std::shared_ptr<CSystem>> m_Systems;
};