#pragma once

#include <span>

class IRenderer;
struct TRenderCommand;
struct TFrameContext;

class IRenderPass
{
public:
  virtual ~IRenderPass() = default;

  virtual void PreExecute(IRenderer &_Renderer, TFrameContext &_FrameContext, std::span<TRenderCommand> _Commands)  = 0;
  virtual void Execute(IRenderer &_Renderer, TFrameContext &_FrameContext, std::span<TRenderCommand> _Commands)     = 0;
  virtual void PostExecute(IRenderer &_Renderer, TFrameContext &_FrameContext, std::span<TRenderCommand> _Commands) = 0;
  virtual bool Accepts(const TRenderCommand &_Command) const                                                        = 0;
  virtual bool IsAvailable() const                                                                                  = 0;
};