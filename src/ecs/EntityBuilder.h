#pragma once

#include "Coordinator.h"

class CEntityBuilder
{
public:
  CEntityBuilder(CCoordinator *_Coordinator) : m_Coordinator(_Coordinator), m_Entity(m_Coordinator->CreateEntity())
  {
  }

  template <typename T>
  void AddComponent(T &&_Component)
  {
    m_Coordinator->AddComponent(m_Entity, std::forward<T>(_Component));
  }

  ecs::TEntity GetEntity() const
  {
    return m_Entity;
  }

private:
  CCoordinator *m_Coordinator;
  ecs::TEntity  m_Entity;
};