#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include <events/EventsListener.h>
#include <common/Sharable.h>
#include <common/MathTypes.h>

class CShader;
struct TRenderTarget;

class CTAARenderPass : public CSharable<CTAARenderPass>,
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
  explicit CTAARenderPass(TVector2i _Viewport);

  ERenderPassType GetType() const override
  {
    return ERenderPassType::TAA;
  }

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;
  bool NeedsCommands() const override;

  void OnEvent(const TEvent &_Event) override;

private:
  using CSharable<CTAARenderPass>::Create;

  void SubscribeToEvents();
  void InitHistoryTargets(const TVector2i &_Viewport);

private:
  static constexpr inline int HISTORY_TARGETS_COUNT = 2;

  std::shared_ptr<CShader>       m_Shader;
  std::shared_ptr<TRenderTarget> m_HistoryTargets[HISTORY_TARGETS_COUNT];

  int32_t m_HistoryIndex;
};
