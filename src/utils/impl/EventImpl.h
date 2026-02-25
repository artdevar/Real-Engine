#pragma once

#include "events/Events.h"
#include "events/Event.h"

namespace event
{

namespace impl
{

template <typename T>
TEvent ConstructEvent(TEventType _EventType, T &&_Value)
{
  return TEvent{
      .Value = std::forward<T>(_Value),
      .Type  = _EventType,
  };
}

} // namespace impl

template <typename T>
void Notify(TEventType _Event, T &&_Value)
{
  TEvent Event = impl::ConstructEvent(_Event, std::forward<T>(_Value));
  Notify(std::move(Event));
}

} // namespace event