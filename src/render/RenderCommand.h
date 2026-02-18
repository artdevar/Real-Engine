#pragma once

#include "Buffer.h"
#include "ShaderTypes.h"
#include "RenderTypes.h"
#include <glm/mat4x4.hpp>

struct TMaterialD
{
  uint32_t   BaseColorTexture;
  int32_t    BaseColorTextureTexCoordIndex;
  uint32_t   NormalTexture;
  int32_t    NormalTextureTexCoordIndex;
  uint32_t   MetallicRoughnessTexture;
  int32_t    MetallicRoughnessTextureTexCoordIndex;
  uint32_t   EmissiveTexture;
  int32_t    EmissiveTextureTexCoordIndex;
  uint32_t   SkyboxTexture;
  glm::vec4  BaseColorFactor;
  glm::vec3  EmissiveFactor;
  float      MetallicFactor;
  float      RoughnessFactor;
  float      AlphaCutoff;
  EAlphaMode AlphaMode;
  bool       IsDoubleSided;
};

struct TRenderCommand
{
  using VAOReference = std::reference_wrapper<CVertexArray>;

  TMaterialD     Material;
  VAOReference   VAO;
  glm::mat4      ModelMatrix;
  uint32_t       IndicesCount;
  EIndexType     IndexType;
  EPrimitiveMode PrimitiveMode;
};
