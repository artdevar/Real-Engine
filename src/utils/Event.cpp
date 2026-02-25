#include "Event.h"
#include "events/EventsManager.h"
#include "events/Event.h"
#include "engine/Engine.h"

static CEventsManager *Get()
{
  return CEngine::Instance().GetEventsManager().get();
}

namespace event
{

void Subscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener)
{
  Get()->Subscribe(_Event, std::move(_Listener));
}

void Unsubscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener)
{
  Get()->Unsubscribe(_Event, std::move(_Listener));
}

void Notify(TEvent _Event)
{
  return Get()->Notify(std::move(_Event));
}

void Notify(TEventType _Event)
{
  return Get()->Notify({.Type = _Event});
}

} // namespace event