#pragma once

class CRenderQueue;

class IRenderCollector
{
public:
  virtual ~IRenderCollector() = default;

  virtual void Collect(CRenderQueue &_Queue) = 0;
};