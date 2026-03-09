#pragma once

#include "Core.h"
#include <common/interfaces/Updateable.h>
#include <common/interfaces/Shutdownable.h>
#include <common/containers/UnorderedVector.h>
#include <map>
#include <memory>

class IEventsListener;
struct TEvent;

class CEventsManager final : public IUpdateable,
                             public IShutdownable
{
public:
  CEventsManager();
  ~CEventsManager();

  void Shutdown() override;
  void Update(float _TimeDelta) override;

  void Subscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener);
  void Unsubscribe(TEventType _Event, std::weak_ptr<IEventsListener> _Listener);

  void Notify(TEvent _Event);
  void Unnotify(const TEvent &_Event);

private:
  using ListenersContainer = std::vector<std::weak_ptr<IEventsListener>>;

  std::unordered_map<TEventType, ListenersContainer> m_Listeners;
  CUnorderedVector<TEvent>                           m_PendingEvents;
  bool                                               m_IsUpdateFunctionRunning;
};