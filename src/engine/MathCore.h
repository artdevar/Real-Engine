#pragma once

#include "MathTypes.h"

template <typename T>
    requires std::is_arithmetic_v<T>
T Clamp(T _Value, T _Min, T _Max)
{
    return std::max(_Min, std::min(_Value, _Max));
}