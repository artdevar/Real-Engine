#pragma once

#include "interfaces/RenderCollector.h"
#include "interfaces/FrameDataCollector.h"
#include <ecs/System.h>

namespace ecs
{

class CEnvironmentRenderSystem : public IRenderCollector,
                                 public IFrameDataCollector,
                                 public CSystem
{
public:
  void Collect(TFrameData &_FrameData) override;
  void Collect(CRenderQueue &_Queue) override;

protected:
  void OnEntityAdded(ecs::TEntity _Entity) override;

private:
  bool m_IsEnvironmentPrepared = false;
};

} // namespace ecs
