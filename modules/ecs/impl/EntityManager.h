#pragma once

#include "Core.h"
#include <common/Common.h>
#include <common/StaticArray.h>
#include <common/UnorderedVector.h>

namespace ecs
{

class CEntityManager final
{
  DISABLE_CLASS_COPY(CEntityManager);

public:
  CEntityManager();

  ecs::TEntity CreateEntity();
  void DestroyEntity(ecs::TEntity _Entity);

  void SetSignature(ecs::TEntity _Entity, ecs::TSignature _Signature);
  ecs::TSignature GetSignature(ecs::TEntity _Entity) const;

  const CUnorderedVector<ecs::TEntity> &GetAliveEntities() const;

private:
  bool IsEntityExist(ecs::TEntity _Entity) const;

private:
  CUnorderedVector<ecs::TEntity>                   m_AliveEntities;
  CStaticArray<ecs::TEntity, ecs::MAX_ENTITIES>    m_AvailableEntities;
  CStaticArray<ecs::TSignature, ecs::MAX_ENTITIES> m_Signatures;
};

} // namespace ecs