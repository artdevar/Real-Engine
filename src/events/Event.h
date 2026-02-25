#pragma once

#include "Events.h"
#include <variant>

struct TEvent
{
  using TEventData = std::variant<std::monostate, bool, int, float>;

  TEventData Value;
  TEventType Type;

  template <typename T>
  const T &GetValue() const
  {
    return std::get<T>(Value);
  }
};
