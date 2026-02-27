#pragma once

#include "interfaces/FrameDataCollector.h"
#include <ecs/System.h>
#include <vector>

namespace ecs
{

class CLightingSystem : public IFrameDataCollector,
                        public CSystem
{
public:
  void Collect(TFrameData &_FrameData) override;
};

} // namespace ecs
