#pragma once

#include "Core.h"

template <typename T>
class CPasskey
{
  DISABLE_CLASS_COPY(CPasskey);

  friend T;

  CPasskey() = default;
  explicit CPasskey(const T *) noexcept
  {
  }
};

template <typename T>
CPasskey(const T *) -> CPasskey<T>;