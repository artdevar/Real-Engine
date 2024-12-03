#pragma once

#include "CommonECS.h"
#include <array>
#include <queue>
#include <vector>

class CEntityManager
{
public:

  CEntityManager();

  CEntityManager(const CEntityManager &) = delete;

  ecs::TEntity CreateEntity();

  void DestroyEntity(ecs::TEntity _Entity);

  const std::vector<ecs::TEntity> & GetExistingEntities() const;

  void SetSignature(ecs::TEntity _Entity, ecs::TSignature _Signature);

  ecs::TSignature GetSignature(ecs::TEntity _Entity) const;

private:

  std::queue<ecs::TEntity>                       m_AvailableEntities;
  std::vector<ecs::TEntity>                      m_ExistingEntities;
  std::array<ecs::TSignature, ecs::MAX_ENTITIES> m_Signatures;
};