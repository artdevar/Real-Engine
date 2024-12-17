#pragma once

#include "CommonECS.h"
#include "utils/StaticArray.h"
#include <unordered_map>
#include <cassert>

// The one instance of virtual inheritance in the entire implementation.
// An interface is needed so that the ComponentManager (seen later)
// can tell a generic ComponentArray that an entity has been destroyed
// and that it needs to update its array mappings.
class IComponentArray
{
public:
  virtual ~IComponentArray() = default;
  virtual void EntityDestroyed(ecs::TEntity _Entity) = 0;
};

template<typename T>
class CComponentArray :
  public IComponentArray
{
public:

  void InsertData(ecs::TEntity _Entity, T && _Component)
  {
    assert(!IsDataExist(_Entity) && "Component added to same entity more than once.");

    const size_t NewIndex = m_ComponentArray.GetActualSize();
    m_ComponentArray.PushBack(std::forward<T>(_Component));

    m_EntityToIndexMap[_Entity]  = NewIndex;
    m_IndexToEntityMap[NewIndex] = _Entity;
  }

  void RemoveData(ecs::TEntity _Entity)
  {
    assert(IsDataExist(_Entity) && "Removing non-existent component.");

    const size_t IndexOfRemovedEntity = m_EntityToIndexMap[_Entity];
    const size_t IndexOfLastElement   = m_ComponentArray.GetActualSize() - 1;

    m_ComponentArray[IndexOfRemovedEntity] = std::move(m_ComponentArray[IndexOfLastElement]);
    m_ComponentArray.PopBack();

    const ecs::TEntity EntityOfLastElement = m_IndexToEntityMap[IndexOfLastElement];
    m_EntityToIndexMap[EntityOfLastElement] = IndexOfRemovedEntity;
    m_IndexToEntityMap[IndexOfRemovedEntity] = EntityOfLastElement;

    m_EntityToIndexMap.erase(_Entity);
    m_IndexToEntityMap.erase(IndexOfLastElement);
  }

  T & GetData(ecs::TEntity _Entity)
  {
    assert(IsDataExist(_Entity) && "Retrieving non-existent component.");

    return m_ComponentArray[m_EntityToIndexMap[_Entity]];
  }

  T * GetDataSafe(ecs::TEntity _Entity)
  {
    auto Iter = m_EntityToIndexMap.find(_Entity);
    if (Iter == m_EntityToIndexMap.end())
      return nullptr;

    return &(m_ComponentArray[Iter->second]);
  }

  bool IsDataExist(ecs::TEntity _Entity) const
  {
    return m_EntityToIndexMap.contains(_Entity);
  }

  void EntityDestroyed(ecs::TEntity _Entity) override
  {
    if (m_EntityToIndexMap.contains(_Entity))
      RemoveData(_Entity);
  }

private:

  CStaticArray<T, ecs::MAX_ENTITIES>       m_ComponentArray;
  std::unordered_map<ecs::TEntity, size_t> m_EntityToIndexMap;
  std::unordered_map<size_t, ecs::TEntity> m_IndexToEntityMap;
};