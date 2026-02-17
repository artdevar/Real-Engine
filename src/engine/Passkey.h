#pragma once

#include "utils/Common.h"

template <typename T>
class CPasskey
{
  friend T;
  CPasskey() = default;
  explicit CPasskey(const T *) noexcept
  {
  }
  DISABLE_CLASS_COPY(CPasskey);
};

template <typename T>
CPasskey(const T *) -> CPasskey<T>;