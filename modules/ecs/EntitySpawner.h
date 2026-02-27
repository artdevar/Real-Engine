#pragma once

#include "Coordinator.h"
#include <common/Common.h>
#include <vector>

namespace ecs
{

class CEntitySpawner final
{
  DISABLE_CLASS_COPY(CEntitySpawner);

public:
  CEntitySpawner(CCoordinator &_Coordinator) :
      m_Coordinator(_Coordinator)
  {
  }

  ecs::TEntity Spawn()
  {
    ecs::TEntity Entity = m_Coordinator.CreateEntity();

    for (auto &Action : m_Actions)
      Action(m_Coordinator, Entity);

    return Entity;
  }

  template <typename T>
  CEntitySpawner &AddComponent(T &&_Component)
  {
    using RawT = std::decay_t<T>;

    std::shared_ptr<RawT> Component = std::make_shared<RawT>(std::forward<T>(_Component));
    m_Actions.emplace_back([Component = std::move(Component)](CCoordinator &_Coordinator, ecs::TEntity _Entity) mutable {
      _Coordinator.AddComponent(_Entity, std::forward<RawT>(*Component));
    });

    return *this;
  }

private:
  CCoordinator &m_Coordinator;

  std::vector<std::function<void(CCoordinator &, ecs::TEntity)>> m_Actions;
};

} // namespace ecs