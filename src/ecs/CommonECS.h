#pragma once

#include <bitset>
#include <cstdint>

namespace ecs
{

using TEntity        = std::uint32_t;
using TComponentType = std::uint8_t;

constexpr inline TEntity        MAX_ENTITIES   = 5000;
constexpr inline TComponentType MAX_COMPONENTS = 32;

using TSignature = std::bitset<MAX_COMPONENTS>;

} // namespace ecs