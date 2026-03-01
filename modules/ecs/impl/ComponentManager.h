#pragma once

#include "Core.h"
#include "Utils.h"
#include "ComponentArray.h"
#include "ComponentView.h"
#include <common/UnorderedVector.h>
#include <memory>
#include <unordered_map>

namespace ecs
{

class CComponentManager
{
public:
  template <typename T>
  void RegisterComponent()
  {
    const TTypeID TypeID = utils::GetComponentTypeID<T>();

    assert(!m_ComponentTypes.contains(TypeID) && "Registering component type more than once.");

    m_ComponentTypes.emplace(TypeID, m_NextComponentType);
    m_ComponentArrays.emplace(TypeID, std::make_shared<CComponentArray<T>>());
    ++m_NextComponentType;
  }

  template <typename T>
  TComponentType GetComponentType()
  {
    const TTypeID TypeID = utils::GetComponentTypeID<T>();
    assert(m_ComponentTypes.contains(TypeID) && "Component not registered before use.");

    return m_ComponentTypes[TypeID];
  }

  template <typename T>
  void AddComponent(TEntity _Entity, T &&_Component)
  {
    GetComponentArray<T>()->InsertData(_Entity, std::forward<T>(_Component));
  }

  template <typename T>
  void RemoveComponent(TEntity _Entity)
  {
    GetComponentArray<T>()->RemoveData(_Entity);
  }

  template <typename T>
  T &GetComponent(TEntity _Entity)
  {
    return GetComponentArray<T>()->GetData(_Entity);
  }

  template <typename T>
  bool DoesComponentExist(TEntity _Entity)
  {
    return GetComponentArray<T>()->IsDataExist(_Entity);
  }

  void EntityDestroyed(TEntity _Entity)
  {
    for (const auto &[TypeID, Component] : m_ComponentArrays)
      Component->EntityDestroyed(_Entity);
  }

  CUnorderedVector<TComponentView> GetEntityComponents(TEntity _Entity) const
  {
    CUnorderedVector<TComponentView> Components;

    for (const auto &[TypeID, Component] : m_ComponentArrays)
    {
      if (void *RawComponent = Component->GetRawComponent(_Entity); RawComponent != nullptr)
        Components.Push(TComponentView{Component->GetComponentName(), TypeID, RawComponent});
    }

    return Components;
  }

private:
  template <typename T>
  std::shared_ptr<CComponentArray<T>> GetComponentArray()
  {
    const TTypeID TypeID = utils::GetComponentTypeID<T>();

    assert(m_ComponentTypes.contains(TypeID) && "Component not registered before use.");

    return std::static_pointer_cast<CComponentArray<T>>(m_ComponentArrays[TypeID]);
  }

private:
  std::unordered_map<TTypeID, TComponentType>                   m_ComponentTypes;
  std::unordered_map<TTypeID, std::shared_ptr<IComponentArray>> m_ComponentArrays;

  TComponentType m_NextComponentType{};
};

} // namespace ecs