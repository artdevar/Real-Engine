#include "ecs/systems/LightingSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "render/FrameData.h"

namespace ecs
{

void CLightingSystem::Collect(TFrameData &_FrameData)
{
  for (ecs::TEntity Entity : m_Entities)
  {
    auto &Light = m_Coordinator->GetComponent<TLightComponent>(Entity);

    _FrameData.Lights.emplace_back(TLight{
        .Type      = Light.Type,
        .Direction = Light.Direction,
        .Ambient   = Light.Ambient,
        .Diffuse   = Light.Diffuse,
        .Specular  = Light.Specular,
    });
  }
}

} // namespace ecs
