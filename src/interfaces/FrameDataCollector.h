#pragma once

struct TFrameData;

class IFrameDataCollector
{
public:
  virtual ~IFrameDataCollector() = default;

  virtual void Collect(TFrameData &_FrameData) = 0;
};