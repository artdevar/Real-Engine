#pragma once

#include <glm/mat4x4.hpp>
#include "engine/MathTypes.h"
#include "Buffer.h"

struct TRenderContext
{
  std::reference_wrapper<CFrameBuffer> SceneFrameBuffer;
  uint32_t                             RenderTexture;

  glm::vec3 CameraPosition;
  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  glm::mat4 ViewProjectionMatrix;
  glm::mat4 LightSpaceMatrix;
  uint32_t  ShadowMap;
};