#pragma once

#include "interfaces/RenderPass.h"
#include "render/RenderContext.h"
#include <common/Sharable.h>

class CShader;

class CTransparentRenderPass : public CSharable<CTransparentRenderPass>,
                               public IRenderPass
{
public:
  explicit CTransparentRenderPass();

  void PreExecute(IRenderer &_Renderer, TRenderContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;

private:
  std::shared_ptr<CShader> m_Shader;
};