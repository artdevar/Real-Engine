#pragma once

#include "ecs/SystemManager.h"
#include "interfaces/RenderCollector.h"

namespace ecs
{

class CSkyboxRenderSystem : public IRenderCollector,
                            public CSystem
{
public:
  void Collect(CRenderQueue &_Queue) override;
};

} // namespace ecs
