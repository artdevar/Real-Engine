#pragma once

#include "Events.h"
#include "interfaces/Updateable.h"
#include "interfaces/Shutdownable.h"
#include <common/UnorderedVector.h>
#include <map>

class IEventsListener;
struct TEvent;

class CEventsManager final : public IUpdateable,
                             public IShutdownable
{
public:
  void Shutdown() override;

  void Subscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener);
  void Unsubscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener);

  void Notify(TEvent _Event);
  void Unnotify(const TEvent &_Event);

private:
  bool ShouldBeUpdated() const override;
  void UpdateInternal(float _TimeDelta) override;

private:
  using ListenersContainer = std::vector<std::weak_ptr<IEventsListener>>;

  std::unordered_map<TEventType, ListenersContainer> m_Listeners;
  CUnorderedVector<TEvent>                           m_PendingEvents;
};