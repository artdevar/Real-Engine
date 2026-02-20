#pragma once

#include "interfaces/RenderPass.h"
#include "render/RenderContext.h"

class CShader;

class TransparentRenderPass : public IRenderPass
{
public:
  explicit TransparentRenderPass(std::shared_ptr<CShader> _Shader);

  void PreExecute(IRenderer &_Renderer, TRenderContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;

private:
  std::weak_ptr<CShader> m_Shader;
};