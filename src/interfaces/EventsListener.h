#pragma once

struct TEvent;

class IEventsListener
{
public:
  virtual ~IEventsListener() = default;

  virtual void OnEvent(const TEvent &_Event) = 0;
};