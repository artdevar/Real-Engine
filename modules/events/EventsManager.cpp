#include "EventsManager.h"
#include "EventsListener.h"

CEventsManager::CEventsManager() :
    m_IsInUpdate(false)
{
}

CEventsManager::~CEventsManager() = default;

void CEventsManager::Shutdown()
{
  m_Listeners.clear();
  m_PendingEvents.clear();
}

void CEventsManager::Update(float _TimeDelta)
{
  m_IsInUpdate = true;

  if (m_PendingEvents.empty())
    return;

  std::vector<TEvent> EventsToProcess = std::move(m_PendingEvents);
  for (const TEvent &Event : EventsToProcess)
  {
    auto ItListeners = m_Listeners.find(Event.Type);
    if (ItListeners == m_Listeners.end())
      continue;

    for (auto ItListener = ItListeners->second.begin(); ItListener != ItListeners->second.end();)
    {
      if (std::shared_ptr<IEventsListener> LockedListener = ItListener->lock())
      {
        LockedListener->OnEvent(Event);
        ++ItListener;
      }
      else
      {
        ItListener = ItListeners->second.erase(ItListener);
      }
    }
  }

  m_IsInUpdate = false;
}

void CEventsManager::Subscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener)
{
  auto      &Listeners           = m_Listeners[_Event];
  const bool IsAlreadySubscribed = std::find_if(Listeners.begin(), Listeners.end(), [&_Listener](const std::weak_ptr<IEventsListener> &L) {
                                     return !L.owner_before(_Listener) && !_Listener.owner_before(L);
                                   }) != Listeners.end();

  if (IsAlreadySubscribed)
    return;

  Listeners.push_back(std::move(_Listener));
}

void CEventsManager::Unsubscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener)
{
  auto      &Listeners = m_Listeners[_Event];
  const auto Iter      = std::find_if(Listeners.begin(), Listeners.end(), [&_Listener](const std::weak_ptr<IEventsListener> &L) {
    return !L.owner_before(_Listener) && !_Listener.owner_before(L);
  });

  if (Iter != Listeners.end())
    Listeners.erase(Iter);
}

void CEventsManager::Notify(TEvent _Event)
{
  auto Iter = std::find_if(m_PendingEvents.begin(), m_PendingEvents.end(), [&_Event](const TEvent &E) {
    return E.Type == _Event.Type;
  });

  if (Iter == m_PendingEvents.end())
    m_PendingEvents.push_back(std::move(_Event));
  else
    Iter->Value = std::move(_Event.Value);
}

void CEventsManager::Unnotify(const TEvent &_Event)
{
  auto Iter = std::find_if(m_PendingEvents.begin(), m_PendingEvents.end(), [&_Event](const TEvent &E) {
    return E.Type == _Event.Type;
  });

  if (Iter != m_PendingEvents.end())
    m_PendingEvents.erase(Iter);
}
