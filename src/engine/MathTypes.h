#pragma once

template <typename T>
    requires std::is_arithmetic_v<T>
struct TVector2
{
    constexpr TVector2() : X{}, Y{}
    {
    }

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr TVector2(U x, U y) : X(static_cast<T>(x)), Y(static_cast<T>(y))
    {
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
    constexpr TVector3() : X{}, Y{}, Z{}
    {
    }

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr TVector3(U x, U y, U z) : X(static_cast<T>(x)), Y(static_cast<T>(y)), Z(static_cast<T>(z))
    {
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
    constexpr TVector4() : X{}, Y{}, Z{}, W{}
    {
    }

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr TVector4(U x, U y, U z, U w) : X(static_cast<T>(x)), Y(static_cast<T>(y)), Z(static_cast<T>(z)), W(static_cast<T>(w))
    {
    }

    T X;
    T Y;
    T Z;
    T W;
};

using TVector4i = TVector4<int>;
using TVector4f = TVector4<float>;