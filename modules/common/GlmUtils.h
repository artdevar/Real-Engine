#pragma once

#include "MathTypes.h"
#include <glm/vec4.hpp>

namespace glm
{

template <typename T>
inline vec2 make_vec2(const TVector2<T> &_Vec)
{
  return {_Vec.X, _Vec.Y};
}

template <typename T>
inline vec3 make_vec3(const TVector3<T> &_Vec)
{
  return {_Vec.X, _Vec.Y, _Vec.Z};
}

template <typename T>
inline vec4 make_vec4(const TVector4<T> &_Vec)
{
  return {_Vec.X, _Vec.Y, _Vec.Z, _Vec.W};
}

inline vec4 make_vec4(const TColor &_Color)
{
  return {_Color.R, _Color.G, _Color.B, _Color.A};
}

} // namespace glm
