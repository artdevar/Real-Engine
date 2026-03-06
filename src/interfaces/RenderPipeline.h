#pragma once

#include <cstdint>
#include "Shutdownable.h"

class IRenderer;
class CRenderQueue;
struct TFrameData;

class IRenderPipeline : public IShutdownable
{
public:
  virtual ~IRenderPipeline() = default;

  virtual void Init()                                                                    = 0;
  virtual void Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer) = 0;

  virtual uint32_t GetDrawCallsCount() const  = 0;
  virtual uint32_t GetVerticesCount() const   = 0;
  virtual uint32_t GetIndicesCount() const    = 0;
  virtual uint32_t GetTrianglesCount() const  = 0;
  virtual uint32_t GetLinesCount() const      = 0;
  virtual uint32_t GetPointsCount() const     = 0;
  virtual uint32_t GetRenderTextureID() const = 0;
};
