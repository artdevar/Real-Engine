#pragma once

#include "Events.h"
#include "engine/MathTypes.h"
#include <variant>

struct TEvent
{
  using TEventData = std::variant<std::monostate, bool, int, float, TVector2i, TVector2f>;

  TEventData Value;
  TEventType Type;

  template <typename T>
  const T &GetValue() const
  {
    return std::get<T>(Value);
  }
};
