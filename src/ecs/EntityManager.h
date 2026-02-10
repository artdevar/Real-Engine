#pragma once

#include "CommonECS.h"
#include "utils/StaticArray.h"
#include "utils/UnorderedVector.h"
#include <vector>

class CEntityManager
{
public:
  CEntityManager();

  CEntityManager(const CEntityManager &) = delete;

  ecs::TEntity CreateEntity();

  void DestroyEntity(ecs::TEntity _Entity);

  const CUnorderedVector<ecs::TEntity> &GetExistingEntities() const;

  void SetSignature(ecs::TEntity _Entity, ecs::TSignature _Signature);

  ecs::TSignature GetSignature(ecs::TEntity _Entity) const;

private:
  CStaticArray<ecs::TEntity, ecs::MAX_ENTITIES> m_AvailableEntities;
  CStaticArray<ecs::TSignature, ecs::MAX_ENTITIES> m_Signatures;
  CUnorderedVector<ecs::TEntity> m_ExistingEntities;
};