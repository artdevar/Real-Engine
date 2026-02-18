#pragma once

#include "interfaces/RenderPass.h"

class CShader;

class SkyboxRenderPass : public IRenderPass
{
public:
  explicit SkyboxRenderPass(std::shared_ptr<CShader> _Shader);

  void PreExecute(IRenderer &_Renderer, TFrameContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  void Execute(IRenderer &_Renderer, TFrameContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  void PostExecute(IRenderer &_Renderer, TFrameContext &_FrameContext, std::span<TRenderCommand> _Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;

private:
  std::weak_ptr<CShader> m_Shader;
};
