#pragma once

#include "interfaces/RenderCollector.h"
#include <ecs/System.h>

namespace ecs
{

class CSkyboxRenderSystem : public IRenderCollector,
                            public CSystem
{
public:
  void Collect(CRenderQueue &_Queue) override;

protected:
  void OnEntityAdded(ecs::TEntity _Entity) override;
};

} // namespace ecs
