#include "EventsManager.h"
#include "interfaces/EventsListener.h"
#include "Event.h"
#include <memory>

void CEventsManager::Shutdown()
{
  m_Listeners.clear();
  m_PendingEvents.Clear();
}

bool CEventsManager::ShouldBeUpdated() const
{
  return !m_PendingEvents.Empty();
}

void CEventsManager::UpdateInternal(float _TimeDelta)
{
  for (const TEvent Event : m_PendingEvents)
  {
    auto Iter = m_Listeners.find(Event.Type);
    if (Iter == m_Listeners.end())
      continue;

    for (const std::weak_ptr<IEventsListener> &Listener : Iter->second)
    {
      if (const std::shared_ptr<IEventsListener> LockedListener = Listener.lock())
        LockedListener->OnEvent(Event);
    }
  }

  m_PendingEvents.Clear();
}

void CEventsManager::Subscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener)
{
  auto      &Listeners           = m_Listeners[_Event];
  const bool IsAlreadySubscribed = Listeners.Contains(_Listener, [&_Listener](const std::weak_ptr<IEventsListener> &L) {
    return !L.owner_before(_Listener) && !_Listener.owner_before(L);
  });

  if (IsAlreadySubscribed)
    return;

  Listeners.Push(std::move(_Listener));
}

void CEventsManager::Unsubscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener)
{
  auto      &Listeners = m_Listeners[_Event];
  const auto Iter      = Listeners.Find(_Listener, [&_Listener](const std::weak_ptr<IEventsListener> &L) {
    return !L.owner_before(_Listener) && !_Listener.owner_before(L);
  });

  if (Iter != Listeners.end())
    Listeners.Erase(Iter);
}

void CEventsManager::Notify(TEvent _Event)
{
  auto Iter = m_PendingEvents.Find(_Event, [&_Event](const TEvent &E) {
    return E.Type == _Event.Type;
  });

  if (Iter == m_PendingEvents.end())
    m_PendingEvents.Push(std::move(_Event));
  else
    Iter->Value = std::move(_Event.Value);
}

void CEventsManager::Unnotify(const TEvent &_Event)
{
  auto Iter = m_PendingEvents.Find(_Event, [&_Event](const TEvent &E) {
    return E.Type == _Event.Type;
  });

  if (Iter != m_PendingEvents.end())
    m_PendingEvents.Erase(Iter);
}
