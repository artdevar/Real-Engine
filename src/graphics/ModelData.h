#pragma once

#include <cstdint>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <map>
#include <string>
#include <vector>
#include "RenderTypes.h"

struct TImage
{
  std::string URI;
};

struct TSampler
{
  ETextureWrap   WrapS;
  ETextureWrap   WrapT;
  ETextureFilter MinFilter;
  ETextureFilter MagFilter;
};

struct TTexture
{
  int ImageIndex    = -1;
  int TexCoordIndex = 0;
  int SamplerIndex  = -1;
};

struct TMaterial
{
  glm::vec4  BaseColorFactor = glm::vec4(1.0f);
  glm::vec3  EmissiveFactor  = glm::vec3(0.0f);
  float      MetallicFactor  = 1.0f;
  float      RoughnessFactor = 1.0f;
  float      AlphaCutoff     = 0.5f;
  EAlphaMode AlphaMode       = EAlphaMode::Opaque;
  bool       IsDoubleSided   = false;

  TTexture BaseColorTexture;
  TTexture MetallicRoughnessTexture;
  TTexture NormalTexture;
  TTexture EmissiveTexture;
};

struct TAttribute
{
  std::vector<uint8_t>    Data;
  EAttributeComponentType ComponentType;
  int                     ByteStride;
  int                     Type;
  bool                    IsNormalized = false;
};

struct TPrimitive
{
  std::map<EAttributeType, TAttribute> Attributes;
  std::vector<uint8_t>                 Indices;
  EPrimitiveMode                       Mode          = EPrimitiveMode::Triangles;
  EIndexType                           IndicesType   = EIndexType::UnsignedInt;
  uint32_t                             IndicesCount  = 0;
  int                                  MaterialIndex = -1;
  uint32_t                             VerticesCount = 0;
};

struct TMesh
{
  std::vector<TPrimitive> Primitives;
};

struct TMatrix
{
  glm::mat4 Value = glm::mat4(1.0f);
};

struct TTRS
{
  glm::vec3 Translation = glm::vec3(0.0f);
  glm::quat Rotation    = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 Scale       = glm::vec3(1.0f);
};

struct TNode
{
  std::vector<int>            Children;
  std::variant<TMatrix, TTRS> TransformData;
  int                         MeshIndex = -1;
};

struct TModelData
{
  std::vector<int>       RootNodes;
  std::vector<TNode>     Nodes;
  std::vector<TMesh>     Meshes;
  std::vector<TMaterial> Materials;
  std::vector<TImage>    Images;
  std::vector<TSampler>  Samplers;
};