#pragma once

#include <limits>
#include <cmath>

namespace math
{

constexpr inline bool AreEqual(int _Lhs, int _Rhs)
{
  return _Lhs == _Rhs;
}

constexpr inline bool AreEqual(float _Lhs, float _Rhs)
{
  const float Diff = _Lhs > _Rhs ? _Lhs - _Rhs : _Rhs - _Lhs;
  return Diff < std::numeric_limits<float>::epsilon();
}

constexpr inline bool AreEqual(double _Lhs, double _Rhs)
{
  const double Diff = _Lhs > _Rhs ? _Lhs - _Rhs : _Rhs - _Lhs;
  return Diff < std::numeric_limits<double>::epsilon();
}

constexpr inline bool IsZero(float _Val)
{
  return AreEqual(_Val, 0.0f);
}

constexpr inline bool IsZero(double _Val)
{
  return AreEqual(_Val, 0.0);
}

} // namespace math