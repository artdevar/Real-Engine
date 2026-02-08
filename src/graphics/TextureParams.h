#pragma once

#include <cstdint>

enum class ETextureWrap : std::uint8_t
{
    Repeat,
    ClampToEdge,
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
    int Width = 0;
    int Height = 0;
    ETextureWrap WrapS = ETextureWrap::Repeat;
    ETextureWrap WrapT = ETextureWrap::Repeat;
    ETextureFilter MinFilter = ETextureFilter::LinearMipmapLinear;
    ETextureFilter MagFilter = ETextureFilter::Linear;
};
