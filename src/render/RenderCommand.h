#pragma once

#include "Buffer.h"
#include "ShaderTypes.h"
#include "RenderTypes.h"
#include <glm/mat4x4.hpp>
#include <bitset>

enum ERenderFlags : uint32_t
{
  ERenderFlags_Transparent,
  ERenderFlags_Opaque,
  ERenderFlags_Skybox,
  ERenderFlags_CastShadow,
  ERenderFlags_ReceiveShadow,
  ERenderFlags_Wireframe,

  ERenderFlags_Count
};

using TRenderFlags = std::bitset<ERenderFlags_Count>;

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
  TMaterialD     Material;
  TSharedVAO     VAO;
  glm::mat4      ModelMatrix;
  uint32_t       IndicesCount;
  EIndexType     IndexType;
  EPrimitiveMode PrimitiveMode;
  TRenderFlags   RenderFlags;
};
