#pragma once

#include "Core.h"
#include <common/UnorderedVector.h>

namespace ecs
{

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

} // namespace ecs