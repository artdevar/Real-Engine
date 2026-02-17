#pragma once

#include "MathTypes.h"
#include <limits>

constexpr inline bool AreEqual(float _Lhs, float _Rhs)
{
  const float Diff = _Lhs > _Rhs ? _Lhs - _Rhs : _Rhs - _Lhs;
  return Diff < std::numeric_limits<float>::epsilon();
}

constexpr inline bool IsZero(float _Val)
{
  return AreEqual(_Val, 0.0f);
}

template <typename T>
constexpr inline float Length(const TVector2<T> &Vec) noexcept
{
  return std::sqrt(Vec.X * Vec.X + Vec.Y * Vec.Y);
}

template <typename T>
constexpr inline float Length2(const TVector2<T> &Vec) noexcept
{
  return Vec.X * Vec.X + Vec.Y * Vec.Y;
}

template <typename T>
constexpr inline float Length(const TVector3<T> &Vec) noexcept
{
  return std::sqrt(Vec.X * Vec.X + Vec.Y * Vec.Y + Vec.Z * Vec.Z);
}

template <typename T>
constexpr inline float Length2(const TVector3<T> &Vec) noexcept
{
  return Vec.X * Vec.X + Vec.Y * Vec.Y + Vec.Z * Vec.Z;
}

template <typename T>
constexpr inline float Length(const TVector4<T> &Vec) noexcept
{
  return std::sqrt(Vec.X * Vec.X + Vec.Y * Vec.Y + Vec.Z * Vec.Z + Vec.W * Vec.W);
}

template <typename T>
constexpr inline float Length2(const TVector4<T> &Vec) noexcept
{
  return Vec.X * Vec.X + Vec.Y * Vec.Y + Vec.Z * Vec.Z + Vec.W * Vec.W;
}

template <Vector T>
constexpr inline T GetNormalized(const T &Vec) noexcept
{
  const float Len = Length(Vec);
  if (IsZero(Len))
    return T();

  return Vec / Len;
}

template <Vector T>
constexpr inline void Normalize(T &Vec) noexcept
{
  const float Len = Length(Vec);
  if (!IsZero(Len))
    Vec /= Len;
}