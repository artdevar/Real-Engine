#pragma once

#include <cstdint>

enum EClearFlags : uint8_t
{
  Color   = 1 << 0,
  Depth   = 1 << 1,
  Stencil = 1 << 2,
  All     = Color | Depth | Stencil
};

enum EAttributeType : uint8_t
{
  Position,
  Normal,
  TexCoords_0,
  TexCoords_1,
  TexCoords_2,
  TexCoords_3,
  Tangent
};

enum EAttributeComponentType : uint8_t
{
  Byte,
  UnsignedByte,
  Short,
  UnsignedShort,
  Int,
  UnsignedInt,
  Float
};

enum class EPrimitiveMode : uint8_t
{
  Points,
  Lines,
  LineLoop,
  LineStrip,
  Triangles,
  TriangleStrip,
  TriangleFan
};

enum class EIndexType : uint8_t
{
  Absent,
  UnsignedByte,
  UnsignedShort,
  UnsignedInt
};

enum class ECullMode : uint8_t
{
  None,
  Front,
  FrontAndBack,
  Back
};

enum class EAlphaMode : uint8_t
{
  Opaque,
  Mask,
  Blend
};

enum class ELightType : uint8_t
{
  Directional,
  Point,
  Spotlight
};

struct TLight
{
  ELightType Type;

  glm::vec3 Direction = glm::vec3(0.0f);
  glm::vec3 Color     = glm::vec3(1.0f);

  float Intensity = 1.0f;
};