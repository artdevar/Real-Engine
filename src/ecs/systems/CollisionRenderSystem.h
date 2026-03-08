#pragma once

#include "interfaces/RenderCollector.h"
#include <ecs/System.h>
#include <memory>

class CVertexArray;
class CVertexBuffer;

namespace ecs
{

class CCollisionRenderSystem : public IRenderCollector,
                               public CSystem
{
public:
  void Collect(CRenderQueue &_Queue) override;

  void OnEntitySelected(ecs::TEntity _Entity);
  void OnEntityDeselected(ecs::TEntity _Entity);

private:
  CUnorderedVector<ecs::TEntity> m_SelectedEntities;
};

} // namespace ecs
