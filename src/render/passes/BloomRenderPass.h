#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include "render/Buffer.h"
#include <common/MathTypes.h>
#include <common/Sharable.h>
#include <events/EventsListener.h>

class CShader;
class CTexture;

class CBloomRenderPass : public CSharable<CBloomRenderPass>,
                         public IEventsListener,
                         public IRenderPass
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
  explicit CBloomRenderPass(TVector2i _Viewport);

  ERenderPassType GetType() const override
  {
    return ERenderPassType::Bloom;
  }

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;
  bool NeedsCommands() const override;

  void OnEvent(const TEvent &_Event) override;

private:
  using CSharable<CBloomRenderPass>::Create;

  void SubscribeToEvents();
  void InitTextures(TVector2i _Size);

private:
  std::shared_ptr<CShader> m_DownsampleShader;
  std::shared_ptr<CShader> m_BlurShader;

  CFrameBuffer                 m_BloomFBO;
  std::shared_ptr<CTexture> m_BloomColor;

  CFrameBuffer                 m_PingPongFBO[2];
  std::shared_ptr<CTexture> m_PingPongColor[2];

  static float m_Threshold;
  static int   m_BlurPasses;
};
