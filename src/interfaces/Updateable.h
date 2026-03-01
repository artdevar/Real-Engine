#pragma once

class IUpdateable
{
public:
  virtual ~IUpdateable() = default;

  virtual void Update(float _TimeDelta) = 0;
};