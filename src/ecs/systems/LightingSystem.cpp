#include "ecs/systems/LightingSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"

namespace ecs
{

std::vector<TLight> CLightingSystem::GetherLightingData() const
{
  std::vector<TLight> LightingData;

  for (ecs::TEntity Entity : m_Entities)
  {
    auto &Light = m_Coordinator->GetComponent<TLightComponent>(Entity);

    LightingData.emplace_back(TLight{
        .Type      = Light.Type,
        .Direction = Light.Direction,
        .Ambient   = Light.Ambient,
        .Diffuse   = Light.Diffuse,
        .Specular  = Light.Specular,
    });
  }

  return LightingData;
}

} // namespace ecs
