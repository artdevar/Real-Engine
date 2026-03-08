#pragma once

#include "MathTypes.h"
#include <glm/vec4.hpp>

namespace glm
{

inline vec4 ToVec4(const TColor &_Color)
{
  return {_Color.R, _Color.G, _Color.B, _Color.A};
}

} // namespace glm
