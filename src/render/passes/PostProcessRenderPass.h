#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include <events/EventsListener.h>
#include <common/Sharable.h>

class CShader;

class CPostProcessRenderPass : public CSharable<CPostProcessRenderPass>,
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
  explicit CPostProcessRenderPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::PostProcess;
  }

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;
  bool NeedsCommands() const override;

  void OnEvent(const TEvent &_Event) override;

private:
  void SubscribeToEvents();

private:
  std::shared_ptr<CShader> m_Shader;

  bool  m_IsFXAAEnabled;
  bool  m_IsHDREnabled;
  bool  m_IsBloomEnabled;
  bool  m_IsGammaCorrectionEnabled;
  float m_HDRExposure;
  float m_BloomIntensity;
  float m_Gamma;
};
