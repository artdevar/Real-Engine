#pragma once

#include <glm/mat4x4.hpp>
#include <common/MathTypes.h>
#include "Buffer.h"

struct TRenderTarget;

struct TRenderContext
{
  TRenderTarget &SceneRenderTarget;
  TRenderTarget &PostProcessRenderTarget;

  glm::vec3 CameraPosition;
  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  glm::mat4 ViewProjectionMatrix;
  glm::mat4 LightSpaceMatrix;
  uint32_t  ShadowMap;
};