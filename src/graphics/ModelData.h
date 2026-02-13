#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

enum EAttributeType : uint32_t
{
  Position,
  Normal,
  TexCoords_0,
  TexCoords_1,
  TexCoords_2,
  TexCoords_3,
  Tangent
};

enum class EModelAlphaMode
{
  Opaque,
  Mask,
  Blend
};

enum class ETextureWrapMode
{
  Repeat,
  ClampToEdge,
  ClampToBorder,
  MirroredRepeat
};

enum class ETextureFilterMode
{
  Nearest,
  Linear,
  LinearMipmapLinear,
  LinearMipmapNearest,
  NearestMipmapNearest,
  NearestMipmapLinear
};

struct TImage
{
  std::string URI;
};

struct TSampler
{
  ETextureWrapMode WrapS;
  ETextureWrapMode WrapT;
  ETextureFilterMode MinFilter;
  ETextureFilterMode MagFilter;
};

struct TTexture
{
  int ImageIndex = -1;
  int TexCoordIndex = 0;
  int SamplerIndex = -1;
};

struct TMaterial
{
  glm::vec4 BaseColorFactor = glm::vec4(1.0f);
  glm::vec3 EmissiveFactor = glm::vec3(0.0f);
  float MetallicFactor = 1.0f;
  float RoughnessFactor = 1.0f;
  float AlphaCutoff = 0.5f;
  EModelAlphaMode AlphaMode = EModelAlphaMode::Opaque;
  bool IsDoubleSided = false;

  TTexture BaseColorTexture;
  TTexture MetallicRoughnessTexture;
  TTexture NormalTexture;
  TTexture EmissiveTexture;
};

struct TAttribute
{
  std::vector<uint8_t> Data;
  int ComponentType;
  int ByteStride;
  int Type;
};

struct TPrimitive
{
  std::map<EAttributeType, TAttribute> Attributes;
  std::vector<uint32_t> Indices;
  int IndicesCount = 0;
  int MaterialIndex = -1;
};

struct TMesh
{
  std::vector<TPrimitive> Primitives;
};

struct TNode
{
  std::vector<int> Children;
  int MeshIndex = -1;

  glm::vec3 Translation = glm::vec3(0.0f);
  glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 Scale = glm::vec3(1.0f);
};

struct TModelData
{
  std::vector<int> RootNodes;
  std::vector<TNode> Nodes;
  std::vector<TMesh> Meshes;
  std::vector<TMaterial> Materials;
  std::vector<TImage> Images;
  std::vector<TSampler> Samplers;
};