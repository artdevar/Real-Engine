#pragma once

#include "interfaces/RenderCollector.h"
#include <ecs/System.h>

namespace ecs
{

class CModelRenderSystem : public IRenderCollector,
                           public CSystem
{
public:
  void Collect(CRenderQueue &_Queue) override;
};

} // namespace ecs