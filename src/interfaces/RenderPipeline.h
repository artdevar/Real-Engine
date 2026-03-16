#pragma once

#include <common/interfaces/Shutdownable.h>
#include <common/MathTypes.h>

class IRenderer;
class CRenderQueue;
struct TFrameData;
enum class ERenderPassType;

class IRenderPipeline : public IShutdownable
{
public:
  virtual ~IRenderPipeline() = default;

  virtual void Init(TVector2i _Viewport)                                                 = 0;
  virtual void Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer) = 0;

  virtual float GetRenderPassTime(ERenderPassType _Type) const = 0;

  virtual uint32_t GetDrawCallsCount() const  = 0;
  virtual uint32_t GetVerticesCount() const   = 0;
  virtual uint32_t GetIndicesCount() const    = 0;
  virtual uint32_t GetTrianglesCount() const  = 0;
  virtual uint32_t GetLinesCount() const      = 0;
  virtual uint32_t GetPointsCount() const     = 0;
  virtual uint32_t GetRenderTextureID() const = 0;
};
