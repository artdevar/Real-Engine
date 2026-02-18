#pragma once

#include "ecs/SystemManager.h"
#include "render/RenderTypes.h"
#include <vector>

namespace ecs
{

class CLightingSystem : public CSystem
{
public:
  std::vector<TLight> GetherLightingData() const;
};

} // namespace ecs
