#pragma once

#include <ctti/type_id.hpp>
#include <bitset>
#include <cstdint>
#include <string>

namespace ecs
{

using TEntity        = std::uint32_t;
using TComponentType = std::uint8_t;

constexpr inline TEntity        MAX_ENTITIES   = 5000;
constexpr inline TComponentType MAX_COMPONENTS = 32;

using TSignature = std::bitset<MAX_COMPONENTS>;
using TTypeID    = ctti::type_id_t;
using TTypeName  = std::string;

} // namespace ecs