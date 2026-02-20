#pragma once

#include "ecs/SystemManager.h"
#include "interfaces/FrameDataCollector.h"
#include "render/RenderTypes.h"
#include <vector>

namespace ecs
{

class CLightingSystem : public IFrameDataCollector,
                        public CSystem
{
public:
  void Collect(TFrameData &_FrameData) override;
  //std::vector<TLight> GetherLightingData() const;
};

} // namespace ecs
