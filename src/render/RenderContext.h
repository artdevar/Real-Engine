#pragma once

#include <glm/glm/fwd.hpp>

struct TRenderTarget;
class CVertexArray;

struct TAAData
{
  glm::mat4 JitteredViewProjectionMatrix;
  glm::mat4 PrevJitteredViewProjectionMatrix;
  glm::vec2 Jitter;
  glm::vec2 PrevJitter;
  uint32_t  VelocityTexture;
  uint32_t  HistoryMap;
};

struct TRenderContext
{
  std::optional<TAAData> TAA;

  CVertexArray &QuadVAO;
  CVertexArray &CubeVAO;

  glm::vec3 CameraPosition;
  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  glm::mat4 ViewProjectionMatrix;
  glm::mat4 LightSpaceMatrix;

  uint32_t ColorTexture;
  uint32_t DepthTexture;
  uint32_t ShadowMap;
  uint32_t BloomMap;
  uint32_t IrradianceMap;
};
