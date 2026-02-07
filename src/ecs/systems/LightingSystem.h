#pragma once

#include "ecs/SystemManager.h"
#include "graphics/ShaderTypes.h"

namespace ecs
{

    class CLightingSystem : public CSystem
    {
    public:
        TShaderLighting ComposeLightingData() const;
    };

}
