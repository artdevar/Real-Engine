#pragma once

#include "events/Events.h"

struct TEvent;
class IEventsListener;

namespace event
{

void Subscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener);
void Unsubscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener);

void Notify(TEvent _Event);
void Notify(TEventType _Event);

template <typename T>
void Notify(TEventType _Event, T &&_Value);

} // namespace event

#include "impl/EventImpl.h"