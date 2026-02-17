#pragma once

#include "TextureParams.h"
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
  Line,
  LineLoop,
  LineStrip,
  Triangles,
  TriangleStrip,
  TriangleFan
};

enum class EIndexType : uint8_t
{
  Byte,
  UnsignedByte,
  Short,
  UnsignedShort,
  Int,
  UnsignedInt,
  Float
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

struct TColor
{
  float R;
  float G;
  float B;
  float A;
};