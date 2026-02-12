
#pragma once

#include <type_traits>

template <typename T>
    requires std::is_arithmetic_v<T>
struct TVector2
{
    constexpr TVector2() noexcept : X{}, Y{}
    {
    }

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr TVector2(U x, U y) noexcept : X(static_cast<T>(x)), Y(static_cast<T>(y))
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

    T X;
    T Y;
};

using TVector2i = TVector2<int>;
using TVector2f = TVector2<float>;

template <typename T>
    requires std::is_arithmetic_v<T>
struct TVector3
{
    constexpr TVector3() noexcept : X{}, Y{}, Z{}
    {
    }

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr TVector3(U x, U y, U z) noexcept : X(static_cast<T>(x)), Y(static_cast<T>(y)), Z(static_cast<T>(z))
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
    constexpr TVector4() noexcept : X{}, Y{}, Z{}, W{}
    {
    }

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr TVector4(U x, U y, U z, U w) noexcept : X(static_cast<T>(x)), Y(static_cast<T>(y)), Z(static_cast<T>(z)), W(static_cast<T>(w))
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