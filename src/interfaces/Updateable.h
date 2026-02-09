#pragma once

class IUpdateable
{
public:
  virtual ~IUpdateable() = default;

  void Update(float _TimeDelta)
  {
    if (ShouldBeUpdated())
      UpdateInternal(_TimeDelta);
  }

protected:
  virtual bool ShouldBeUpdated() const = 0;

private:
  virtual void UpdateInternal(float _TimeDelta) = 0;
};