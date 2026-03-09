#pragma once

#include "RenderPassTypes.h"
#include "render/RenderCommand.h"
#include "interfaces/Renderer.h"
#include "interfaces/RenderPass.h"
#include <events/EventsListener.h>
#include <common/Sharable.h>
#include <vector>

class CTextureBase;
class CShader;

class CShadowRenderPass : public CSharable<CShadowRenderPass>,
                          public IRenderPass,
                          public IEventsListener
{
public:
  template <typename... Args>
  static TSharedPtr Create(Args &&..._Args)
  {
    TSharedPtr Instance = CSharable::Create(std::forward<Args>(_Args)...);
    Instance->SubscribeToEvents();
    return Instance;
  }

public:
  explicit CShadowRenderPass();
  ~CShadowRenderPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::Shadow;
  }

  bool Accepts(const TRenderCommand &_Command) const override;
  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  bool IsAvailable() const override;

  void OnEvent(const TEvent &_Event) override;

private:
  using CSharable<CShadowRenderPass>::Create;

  void SubscribeToEvents();

  static std::shared_ptr<CTextureBase> CreateDepthMap(TVector2i _Size);
  void DestroyDepthMap();

private:
  static const std::string SHADOW_MAP_NAME;

  int                           m_ShadowMapSize;
  std::shared_ptr<CShader>      m_Shader;
  std::shared_ptr<CTextureBase> m_DepthMap;
  CFrameBuffer                  m_DepthMapFBO;
  TVector2i                     m_OldViewport;
};
