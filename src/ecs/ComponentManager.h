#pragma once

#include "CommonECS.h"
#include "ComponentArray.h"
#include <ctti/type_id.hpp>
#include <unordered_map>
#include <memory>

class CComponentManager
{
public:

  template <typename T>
  void RegisterComponent()
  {
    const ctti::type_id_t TypeID = ctti::type_id<T>();

    assert(!m_ComponentTypes.contains(TypeID) && "Registering component type more than once.");

    m_ComponentTypes.emplace(TypeID, m_NextComponentType);
    m_ComponentArrays.emplace(TypeID, std::make_shared<CComponentArray<T>>());
    ++m_NextComponentType;
  }

  template <typename T>
  ecs::TComponentType GetComponentType()
  {
    const ctti::type_id_t TypeID = ctti::type_id<T>();
    assert(m_ComponentTypes.contains(TypeID) && "Component not registered before use.");

    return m_ComponentTypes[TypeID];
  }

  template <typename T>
  void AddComponent(ecs::TEntity _Entity, T && _Component)
  {
    GetComponentArray<T>()->InsertData(_Entity, std::forward<T>(_Component));
  }

  template <typename T>
  void RemoveComponent(ecs::TEntity _Entity)
  {
    GetComponentArray<T>()->RemoveData(_Entity);
  }

  template <typename T>
  T & GetComponent(ecs::TEntity _Entity)
  {
    return GetComponentArray<T>()->GetData(_Entity);
  }

  template <typename T>
  T * GetComponentSafe(ecs::TEntity _Entity)
  {
    return GetComponentArray<T>()->GetDataSafe(_Entity);
  }

  template <typename T>
  bool IsComponentExist(ecs::TEntity _Entity)
  {
    return GetComponentArray<T>()->IsDataExist(_Entity);
  }

  void EntityDestroyed(ecs::TEntity _Entity)
  {
    for (const auto & [TypeID, Component] : m_ComponentArrays)
      Component->EntityDestroyed(_Entity);
  }

private:

  std::unordered_map<ctti::type_id_t, ecs::TComponentType>              m_ComponentTypes;
  std::unordered_map<ctti::type_id_t, std::shared_ptr<IComponentArray>> m_ComponentArrays;

  ecs::TComponentType m_NextComponentType{};

  template <typename T>
  std::shared_ptr<CComponentArray<T>> GetComponentArray()
  {
    const ctti::type_id_t TypeID = ctti::type_id<T>();

    assert(m_ComponentTypes.contains(TypeID) && "Component not registered before use.");

    return std::static_pointer_cast<CComponentArray<T>>(m_ComponentArrays[TypeID]);
  }
};