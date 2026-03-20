#pragma once

#include <glm/mat4x4.hpp>
#include <common/MathTypes.h>
#include "Buffer.h"

struct TRenderTarget;

struct TRenderContext
{
  CVertexArray &QuadVAO;
  CVertexArray &CubeVAO;

  glm::vec3 CameraPosition;
  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  glm::mat4 ViewProjectionMatrix;
  glm::mat4 JitteredViewProjectionMatrix;
  glm::mat4 PreviousViewProjectionMatrix;
  glm::mat4 PreviousJitteredViewProjectionMatrix;
  glm::mat4 LightSpaceMatrix;
  glm::vec2 Jitter;
  glm::vec2 PrevJitter;

  uint32_t ColorTexture;
  uint32_t DepthTexture;
  uint32_t VelocityTexture;
  uint32_t ShadowMap;
  uint32_t BloomMap;
  uint32_t IrradianceMap;
  uint32_t TAAHistoryMap;
};
