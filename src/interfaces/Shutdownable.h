#pragma once

class IShutdownable
{
public:
  virtual ~IShutdownable() = default;

  virtual void Shutdown() = 0;
};