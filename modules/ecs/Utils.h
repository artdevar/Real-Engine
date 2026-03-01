#pragma once

#include "Core.h"
#include <ctti/detailed_nameof.hpp>

namespace ecs::utils
{

template <typename T>
TTypeID GetComponentTypeID()
{
  return ctti::type_id<T>();
}

template <typename T>
TTypeName GetComponentName()
{
  return ctti::detailed_nameof<T>().name().str();
}

} // namespace ecs::utils