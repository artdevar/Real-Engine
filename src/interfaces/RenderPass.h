#pragma once

#include <span>

class IRenderer;
struct TRenderCommand;
struct TRenderContext;
enum class ERenderPassType;

class IRenderPass
{
public:
  virtual ~IRenderPass() = default;

  using CommandsList = std::vector<const TRenderCommand *>;

  virtual ERenderPassType GetType() const                                                                       = 0;
  virtual void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands)  = 0;
  virtual void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands)     = 0;
  virtual void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) = 0;
  virtual bool Accepts(const TRenderCommand &_Command) const                                                    = 0;
  virtual bool IsAvailable() const                                                                              = 0;
  virtual bool NeedsCommands() const                                                                            = 0;
};

;
