#pragma once

#include "Core.h"
#include <common/Sharable.h>
#include <common/interfaces/Updateable.h>
#include <common/interfaces/Shutdownable.h>
#include <unordered_map>
#include <vector>
#include <memory>

class IEventsListener;
struct TEvent;

class CEventsManager final : public CSharable<CEventsManager>,
                             public IUpdateable,
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
  using ListenersList = std::vector<std::weak_ptr<IEventsListener>>;

  std::unordered_map<TEventType, ListenersList> m_Listeners;
  std::vector<TEvent>                           m_PendingEvents;
  bool                                          m_IsInUpdate;
};
