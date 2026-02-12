#pragma once

#include <cstdint>

enum EClearFlags : uint32_t
{
    Color = 1 << 0,
    Depth = 1 << 1,
    Stencil = 1 << 2,
    All = Color | Depth | Stencil
};

enum class EPrimitiveMode
{
    Triangles,
    Lines,
    Points
};

enum class ECullMode
{
    None,
    Front,
    FrontAndBack,
    Back
};

enum class EAlphaMode
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