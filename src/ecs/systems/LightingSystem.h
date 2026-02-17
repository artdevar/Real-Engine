#pragma once

#include "ecs/SystemManager.h"
#include "graphics/ShaderTypes.h"

class IRenderer;

namespace ecs
{

class CLightingSystem : public CSystem
{
public:
  void ComposeLightingData(IRenderer &_Renderer) const;
};

} // namespace ecs
