#pragma once

#include "interfaces/Renderer.h"
#include "render/RenderCommand.h"
#include "interfaces/RenderPass.h"
#include <vector>

class CTextureBase;
class CShader;

class ShadowRenderPass : public IRenderPass
{
public:
  explicit ShadowRenderPass(std::shared_ptr<CShader> _Shader);

  bool Accepts(const TRenderCommand &_Command) const override;
  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  bool IsAvailable() const override;

private:
  static std::shared_ptr<CTextureBase> CreateDepthMap(TVector2i _Size);

private:
  const int                     m_ShadowMapSize;
  std::weak_ptr<CShader>        m_Shader;
  std::shared_ptr<CTextureBase> m_DepthMap;
  CFrameBuffer                  m_DepthMapFBO;
  TVector2i                     m_OldViewport;
};
