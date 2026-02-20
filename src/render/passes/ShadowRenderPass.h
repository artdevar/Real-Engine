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
  explicit ShadowRenderPass(std::shared_ptr<CShader> shader);

  bool Accepts(const TRenderCommand &command) const override;
  void PreExecute(IRenderer &renderer, TRenderContext &renderContext, std::span<TRenderCommand> commands) override;
  void Execute(IRenderer &renderer, TRenderContext &renderContext, std::span<TRenderCommand> commands) override;
  void PostExecute(IRenderer &renderer, TRenderContext &renderContext, std::span<TRenderCommand> commands) override;
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
