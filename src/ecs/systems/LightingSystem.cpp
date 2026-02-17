#include "ecs/systems/LightingSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "interfaces/Renderer.h"
#include "utils/Common.h"
#include <cassert>
#include <cstring>

namespace ecs
{

void CLightingSystem::ComposeLightingData(IRenderer &_Renderer) const
{
  TShaderLighting Lighting;
  std::memset(&Lighting, 0x00, sizeof(Lighting));

  int PointLightIndex = 0;

  for (ecs::TEntity Entity : m_Entities)
  {
    auto &Light = m_Coordinator->GetComponent<TLightComponent>(Entity);

    switch (Light.Type)
    {
    case ELightType::Directional:
      Lighting.LightDirectional.Ambient   = Light.Ambient;
      Lighting.LightDirectional.Diffuse   = Light.Diffuse;
      Lighting.LightDirectional.Specular  = Light.Specular;
      Lighting.LightDirectional.Direction = Light.Direction;
      break;

    default:
      assert(false);
      break;
    }
  }

  _Renderer.SetLightingData(std::move(Lighting));
}

} // namespace ecs
