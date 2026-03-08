#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include "interfaces/EventsListener.h"
#include "render/Buffer.h"
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

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;

  void OnEvent(const TEvent &_Event) override;

private:
  using CSharable<CPostProcessRenderPass>::Create;

  void SubscribeToEvents();

private:
  std::shared_ptr<CShader> m_Shader;

  CVertexArray  m_VAO;
  CVertexBuffer m_VBO;

  bool  m_IsFxaaEnabled;
  bool  m_IsHDREnabled;
  bool  m_IsGammaCorrectionEnabled;
  float m_HDRExposure;
  float m_Gamma;
};