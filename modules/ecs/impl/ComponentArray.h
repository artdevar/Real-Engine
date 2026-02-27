#pragma once

#include "IComponentArray.h"
#include <common/StaticArray.h>
#include <cassert>
#include <unordered_map>

namespace ecs
{

template <typename T>
class CComponentArray : public IComponentArray
{
public:
  void InsertData(TEntity _Entity, T &&_Component)
  {
    assert(!IsDataExist(_Entity) && "Component added to same entity more than once.");

    const size_t NewIndex = m_ComponentArray.GetActualSize();
    m_ComponentArray.PushBack(std::forward<T>(_Component));

    m_EntityToIndexMap[_Entity]  = NewIndex;
    m_IndexToEntityMap[NewIndex] = _Entity;
  }

  void RemoveData(TEntity _Entity)
  {
    assert(IsDataExist(_Entity) && "Removing non-existent component.");

    const size_t IndexOfRemovedEntity = m_EntityToIndexMap[_Entity];
    const size_t IndexOfLastElement   = m_ComponentArray.GetActualSize() - 1;

    m_ComponentArray[IndexOfRemovedEntity] = std::move(m_ComponentArray[IndexOfLastElement]);
    m_ComponentArray.PopBack();

    const ecs::TEntity EntityOfLastElement   = m_IndexToEntityMap[IndexOfLastElement];
    m_EntityToIndexMap[EntityOfLastElement]  = IndexOfRemovedEntity;
    m_IndexToEntityMap[IndexOfRemovedEntity] = EntityOfLastElement;

    m_EntityToIndexMap.erase(_Entity);
    m_IndexToEntityMap.erase(IndexOfLastElement);
  }

  T &GetData(TEntity _Entity)
  {
    assert(IsDataExist(_Entity) && "Retrieving non-existent component.");
    return m_ComponentArray[m_EntityToIndexMap[_Entity]];
  }

  bool IsDataExist(TEntity _Entity) const
  {
    return m_EntityToIndexMap.contains(_Entity);
  }

  void EntityDestroyed(TEntity _Entity) override
  {
    if (m_EntityToIndexMap.contains(_Entity))
      RemoveData(_Entity);
  }

private:
  CStaticArray<T, MAX_ENTITIES>       m_ComponentArray;
  std::unordered_map<TEntity, size_t> m_EntityToIndexMap;
  std::unordered_map<size_t, TEntity> m_IndexToEntityMap;
};

} // namespace ecs