#pragma once

#include <glm/mat4x4.hpp>

struct TRenderContext
{
  glm::vec3 CameraPosition;
  glm::mat4 ViewProjectionMatrix;
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;
  glm::mat4 LightSpaceMatrix;
  uint32_t  ShadowMap;
};