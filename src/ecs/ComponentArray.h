#pragma once

#include "CommonECS.h"
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
    assert(m_EntityToIndexMap.find(_Entity) == m_EntityToIndexMap.end() && "Component added to same entity more than once.");

    const size_t NewIndex = m_Size;

    m_EntityToIndexMap[_Entity]  = NewIndex;
    m_IndexToEntityMap[NewIndex] = _Entity;
    m_ComponentArray[NewIndex]   = std::forward<T>(_Component);

    ++m_Size;
  }

  void RemoveData(ecs::TEntity _Entity)
  {
    assert(m_EntityToIndexMap.find(_Entity) != m_EntityToIndexMap.end() && "Removing non-existent component.");

    size_t indexOfRemovedEntity = m_EntityToIndexMap[_Entity];
    size_t indexOfLastElement   = m_Size - 1;
    m_ComponentArray[indexOfRemovedEntity] = std::move(m_ComponentArray[indexOfLastElement]);

    // Update map to point to moved spot
    ecs::TEntity entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
    m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
    m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

    m_EntityToIndexMap.erase(_Entity);
    m_IndexToEntityMap.erase(indexOfLastElement);

    --m_Size;
  }

  T & GetData(ecs::TEntity _Entity)
  {
    assert(IsDataExist(_Entity) && "Retrieving non-existent component.");

    // Return a reference to the entity's component
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
  // The packed array of components (of generic type T),
  // set to a specified maximum amount, matching the maximum number
  // of entities allowed to exist simultaneously, so that each entity
  // has a unique spot.
  std::array<T, ecs::MAX_ENTITIES>         m_ComponentArray;
  std::unordered_map<ecs::TEntity, size_t> m_EntityToIndexMap;
  std::unordered_map<size_t, ecs::TEntity> m_IndexToEntityMap;

  size_t m_Size = 0;
};