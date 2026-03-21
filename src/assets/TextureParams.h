#pragma once

#include <common/containers/StaticArray.h>
#include <cstdint>
#include <optional>

enum class ETextureType
{
  BasicColor,
  Normal,
  Roughness,
  Occlusion,
  Emissive
};

enum class ETextureWrap : std::uint8_t
{
  Repeat,
  ClampToEdge,
  ClampToBorder,
  MirroredRepeat
};

enum class ETextureFilter : std::uint8_t
{
  Nearest,
  Linear,
  LinearMipmapLinear,
  LinearMipmapNearest,
  NearestMipmapNearest,
  NearestMipmapLinear
};

enum class EInternalFormat : std::uint8_t
{
  R8,
  RG8,
  RGB8,
  RGBA8,
  RG16F,
  RGB16F,
  RGBA16F,
  Depth16,
  Depth24,
  Depth32
};

enum class EFormat : std::uint8_t
{
  Red,
  RG,
  RGB,
  RGBA,
  Depth
};

enum class EType : std::uint8_t
{
  Byte,
  UnsignedByte,
  Short,
  UnsignedShort,
  Int,
  UnsignedInt,
  Float
};

struct TTextureParams
{
  static constexpr inline int BordersCount = 4;

  void *Data = nullptr;

  int Width  = 0;
  int Height = 0;

  EInternalFormat InternalFormat;
  EFormat         Format;
  EType           Type;

  bool  GenerateMipmaps = false;
  bool  HDR             = false;
  bool  sRGB            = false;
  float Anisotropy      = 16.0f;

  std::optional<int>                               Samples;
  std::optional<CStaticArray<float, BordersCount>> BorderColors;

  ETextureWrap   WrapS     = ETextureWrap::Repeat;
  ETextureWrap   WrapT     = ETextureWrap::Repeat;
  ETextureWrap   WrapR     = ETextureWrap::Repeat; // For cubemaps
  ETextureFilter MinFilter = ETextureFilter::LinearMipmapLinear;
  ETextureFilter MagFilter = ETextureFilter::Linear;
};
