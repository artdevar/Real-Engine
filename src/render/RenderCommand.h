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
  ERenderFlags_EquirectangularToCubemap,
  ERenderFlags_IrradianceConvolution,
  ERenderFlags_CastShadow,
  ERenderFlags_ReceiveShadow,
  ERenderFlags_Wireframe,

  ERenderFlags_Count
};

using TRenderFlags = std::bitset<ERenderFlags_Count>;

struct TMaterial
{
  uint32_t   BaseColorTexture;
  int32_t    BaseColorTextureTexCoordIndex;
  uint32_t   NormalTexture;
  int32_t    NormalTextureTexCoordIndex;
  uint32_t   MetallicRoughnessTexture;
  int32_t    MetallicRoughnessTextureTexCoordIndex;
  uint32_t   OcclusionTexture;
  int32_t    OcclusionTextureTexCoordIndex;
  uint32_t   EmissiveTexture;
  int32_t    EmissiveTextureTexCoordIndex;
  glm::vec4  BaseColorFactor;
  glm::vec3  EmissiveFactor;
  float      MetallicFactor;
  float      RoughnessFactor;
  float      OcclusionStrength;
  float      AlphaCutoff;
  EAlphaMode AlphaMode;
  bool       IsDoubleSided;
};

struct TEnvironment
{
  uint32_t SkyboxTexture;
  uint32_t EquirectangularMap;
  uint32_t IrradianceMap;
};

struct TRenderCommand
{
  TMaterial      Material;
  TEnvironment   Environment;
  TSharedVAO     VAO;
  glm::mat4      ModelMatrix;
  uint32_t       IndicesCount;
  EIndexType     IndexType;
  EPrimitiveMode PrimitiveMode;
  TRenderFlags   RenderFlags;
};
