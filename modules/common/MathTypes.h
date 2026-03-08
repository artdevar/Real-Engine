
#pragma once

#include "MathCore.h"
#include <type_traits>

struct TColor
{
  constexpr TColor() noexcept :
      R{},
      G{},
      B{},
      A{}
  {
  }

  constexpr TColor(float _R, float _G, float _B, float _A) noexcept :
      R(_R),
      G(_G),
      B(_B),
      A(_A)
  {
  }

  constexpr bool operator==(const TColor &Other) const noexcept
  {
    return math::AreEqual(R, Other.R) && math::AreEqual(G, Other.G) && math::AreEqual(B, Other.B) && math::AreEqual(A, Other.A);
  }

  constexpr bool operator!=(const TColor &Other) const noexcept
  {
    return !(*this == Other);
  }

  float R;
  float G;
  float B;
  float A;
};

template <typename T>
requires std::is_arithmetic_v<T>
struct TVector2
{
  constexpr TVector2() noexcept :
      X{},
      Y{}
  {
  }

  template <typename U>
  requires std::is_convertible_v<U, T>
  constexpr TVector2(U x, U y) noexcept :
      X(static_cast<T>(x)),
      Y(static_cast<T>(y))
  {
  }

  template <typename U>
  constexpr explicit TVector2(const TVector2<U> &_Other) noexcept :
      X(static_cast<T>(_Other.X)),
      Y(static_cast<T>(_Other.Y))
  {
  }

  constexpr inline TVector2 operator/(float _Scalar) const noexcept
  {
    return TVector2<T>(static_cast<T>(X / _Scalar), static_cast<T>(Y / _Scalar));
  }

  constexpr inline TVector2 &operator/=(float _Scalar) noexcept
  {
    X = static_cast<T>(X / _Scalar);
    Y = static_cast<T>(Y / _Scalar);
    return *this;
  }

  constexpr inline bool operator==(const TVector2 &Other) const noexcept
  {
    return math::AreEqual(X, Other.X) && math::AreEqual(Y, Other.Y);
  }

  constexpr inline bool operator!=(const TVector2 &Other) const noexcept
  {
    return !(*this == Other);
  }

  T X;
  T Y;
};

using TVector2i = TVector2<int>;
using TVector2f = TVector2<float>;

using TSize2i = TVector2i;
using TSize2f = TVector2f;

using TPoint2i = TVector2i;
using TPoint2f = TVector2f;

template <typename T>
requires std::is_arithmetic_v<T>
struct TVector3
{
  constexpr TVector3() noexcept :
      X{},
      Y{},
      Z{}
  {
  }

  template <typename U>
  requires std::is_convertible_v<U, T>
  constexpr TVector3(U x, U y, U z) noexcept :
      X(static_cast<T>(x)),
      Y(static_cast<T>(y)),
      Z(static_cast<T>(z))
  {
  }

  template <typename U>
  constexpr explicit TVector3(const TVector3<U> &_Other) noexcept :
      X(static_cast<T>(_Other.X)),
      Y(static_cast<T>(_Other.Y)),
      Z(static_cast<T>(_Other.Z))
  {
  }

  constexpr inline TVector3 operator/(float _Scalar) const noexcept
  {
    return TVector3<T>(static_cast<T>(X / _Scalar), static_cast<T>(Y / _Scalar), static_cast<T>(Z / _Scalar));
  }

  constexpr inline TVector3 &operator/=(float _Scalar) noexcept
  {
    X = static_cast<T>(X / _Scalar);
    Y = static_cast<T>(Y / _Scalar);
    Z = static_cast<T>(Z / _Scalar);
    return *this;
  }

  constexpr inline bool operator==(const TVector3 &Other) const noexcept
  {
    return math::AreEqual(X, Other.X) && math::AreEqual(Y, Other.Y) && math::AreEqual(Z, Other.Z);
  }

  constexpr inline bool operator!=(const TVector3 &Other) const noexcept
  {
    return !(*this == Other);
  }

  T X;
  T Y;
  T Z;
};

using TVector3i = TVector3<int>;
using TVector3f = TVector3<float>;

template <typename T>
requires std::is_arithmetic_v<T>
struct TVector4
{
  constexpr TVector4() noexcept :
      X{},
      Y{},
      Z{},
      W{}
  {
  }

  template <typename U>
  constexpr explicit TVector4(const TVector4<U> &_Other) noexcept :
      X(static_cast<T>(_Other.X)),
      Y(static_cast<T>(_Other.Y)),
      Z(static_cast<T>(_Other.Z)),
      W(static_cast<T>(_Other.W))
  {
  }

  constexpr inline TVector4 operator/(float _Scalar) const noexcept
  {
    return TVector4<T>(static_cast<T>(X / _Scalar), static_cast<T>(Y / _Scalar), static_cast<T>(Z / _Scalar), static_cast<T>(W / _Scalar));
  }

  constexpr inline TVector4 &operator/=(float _Scalar) noexcept
  {
    X = static_cast<T>(X / _Scalar);
    Y = static_cast<T>(Y / _Scalar);
    Z = static_cast<T>(Z / _Scalar);
    W = static_cast<T>(W / _Scalar);
    return *this;
  }

  constexpr inline bool operator==(const TVector4 &Other) const noexcept
  {
    return math::AreEqual(X, Other.X) && math::AreEqual(Y, Other.Y) && math::AreEqual(Z, Other.Z) && math::AreEqual(W, Other.W);
  }

  constexpr inline bool operator!=(const TVector4 &Other) const noexcept
  {
    return !(*this == Other);
  }

  T X;
  T Y;
  T Z;
  T W;
};

using TVector4i = TVector4<int>;
using TVector4f = TVector4<float>;

template <typename T>
struct IsVector : std::false_type
{
};

template <typename T>
struct IsVector<TVector2<T>> : std::true_type
{
};
template <typename T>
struct IsVector<TVector3<T>> : std::true_type
{
};
template <typename T>
struct IsVector<TVector4<T>> : std::true_type
{
};

template <typename T>
concept Vector = IsVector<std::remove_cvref_t<T>>::value;

template <typename T>
requires std::is_arithmetic_v<T>
struct TRect
{
  constexpr TRect() noexcept :
      Position{},
      Size{}
  {
  }

  template <typename U>
  requires std::is_convertible_v<U, T>
  constexpr TRect(U _X, U _Y, U _Width, U _Height) noexcept :
      Position(static_cast<T>(_X), static_cast<T>(_Y)),
      Size(static_cast<T>(_Width), static_cast<T>(_Height))
  {
  }

  TVector2<T> Position;
  TVector2<T> Size;
};

using TRecti = TRect<int>;
using TRectf = TRect<float>;

namespace math
{

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

} // namespace math