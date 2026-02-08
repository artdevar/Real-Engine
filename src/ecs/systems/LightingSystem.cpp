#include "ecs/systems/LightingSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "utils/Common.h"
#include <cstring>
#include <cassert>

namespace ecs
{

    TShaderLighting CLightingSystem::ComposeLightingData() const
    {
        TShaderLighting Lighting;
        std::memset(&Lighting, 0x00, sizeof(Lighting));

        int PointLightIndex = 0;

        for (ecs::TEntity Entity : m_Entities)
        {
            auto &Light = m_Coordinator->GetComponent<TLightComponent>(Entity);

            switch (Light.Type)
            {
            case ELightType::Point:
                Lighting.LightPoints[PointLightIndex].Ambient = Light.Ambient;
                Lighting.LightPoints[PointLightIndex].Position = Light.Position;
                Lighting.LightPoints[PointLightIndex].Diffuse = Light.Diffuse;
                Lighting.LightPoints[PointLightIndex].Specular = Light.Specular;
                Lighting.LightPoints[PointLightIndex].Constant = Light.Constant;
                Lighting.LightPoints[PointLightIndex].Linear = Light.Linear;
                Lighting.LightPoints[PointLightIndex].Quadratic = Light.Quadratic;
                ++PointLightIndex;
                break;

            case ELightType::Directional:
                Lighting.LightDirectional.Ambient = Light.Ambient;
                Lighting.LightDirectional.Diffuse = Light.Diffuse;
                Lighting.LightDirectional.Specular = Light.Specular;
                Lighting.LightDirectional.Direction = Light.Direction;
                break;

            case ELightType::Spotlight:
                Lighting.LightSpot.Ambient = Light.Ambient;
                Lighting.LightSpot.Position = Light.Position;
                Lighting.LightSpot.Direction = Light.Direction;
                Lighting.LightSpot.Diffuse = Light.Diffuse;
                Lighting.LightSpot.Specular = Light.Specular;
                Lighting.LightSpot.CutOff = Light.CutOff;
                Lighting.LightSpot.OuterCutOff = Light.OuterCutOff;
                Lighting.LightSpot.Constant = Light.Constant;
                Lighting.LightSpot.Linear = Light.Linear;
                Lighting.LightSpot.Quadratic = Light.Quadratic;
                break;

            default:
                assert(false);
                break;
            }
        }

        Lighting.PointLightsCount = PointLightIndex;

        return Lighting;
    }

}
