#pragma once

#include <limits>

namespace math
{

constexpr inline bool AreEqual(float _Lhs, float _Rhs)
{
  const float Diff = _Lhs > _Rhs ? _Lhs - _Rhs : _Rhs - _Lhs;
  return Diff < std::numeric_limits<float>::epsilon();
}

constexpr inline bool IsZero(float _Val)
{
  return AreEqual(_Val, 0.0f);
}

} // namespace math