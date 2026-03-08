#pragma once

#include "Events.h"
#include <common/MathTypes.h>
#include <variant>

struct TEvent
{
  using TEventData = std::variant<std::monostate, bool, int, unsigned int, float, TVector2i, TVector2f, TColor>;

  TEventData Value;
  TEventType Type;

  template <typename T>
  const T &GetValue() const
  {
    return std::get<T>(Value);
  }
};
