#pragma once

#include "utils/StaticArray.h"
#include <cstdint>
#include <optional>

enum class ETextureType
{
  BasicColor,
  Normal,
  Roughness,
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

struct TTextureParams
{
  static constexpr inline int BordersCount = 4;

  int Width  = 0;
  int Height = 0;

  int InternalFormat = 0;
  int Format         = 0;
  int Type           = 0;

  std::optional<int> Samples;

  std::optional<CStaticArray<float, BordersCount>> BorderColors;
  ETextureWrap                                     WrapS     = ETextureWrap::Repeat;
  ETextureWrap                                     WrapT     = ETextureWrap::Repeat;
  ETextureWrap                                     WrapR     = ETextureWrap::Repeat; // For cubemaps
  ETextureFilter                                   MinFilter = ETextureFilter::LinearMipmapLinear;
  ETextureFilter                                   MagFilter = ETextureFilter::Linear;
};
