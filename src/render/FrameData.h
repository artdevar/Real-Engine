#pragma once

#include "RenderTypes.h"
#include <glm/fwd.hpp>
#include <vector>

struct TFrameData
{
  struct TLight
  {
    ELightType Type;

    glm::vec3 Direction = glm::vec3(0.0f);
    glm::vec3 Color     = glm::vec3(1.0f);

    float Intensity = 1.0f;
  };

  struct TEnvironment
  {
    uint32_t SkyboxTexture      = 0;
    uint32_t EquirectangularMap = 0;
    uint32_t IrradianceMap      = 0;
  };

  std::vector<TLight> Lights;
  TEnvironment        Environment;
};
