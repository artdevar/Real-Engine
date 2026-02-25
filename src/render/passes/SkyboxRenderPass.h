#pragma once

#include "interfaces/RenderPass.h"
#include "interfaces/Sharable.h"
#include "render/RenderContext.h"

class CShader;

class CSkyboxRenderPass : public CSharable<CSkyboxRenderPass>,
                          public IRenderPass
{
public:
  explicit CSkyboxRenderPass(std::shared_ptr<CShader> _Shader);

  void PreExecute(IRenderer &_Renderer, TRenderContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;

private:
  std::shared_ptr<CShader> m_Shader;
};
