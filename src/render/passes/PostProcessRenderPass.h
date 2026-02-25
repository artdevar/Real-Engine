#pragma once

#include "interfaces/RenderPass.h"
#include "interfaces/EventsListener.h"
#include "interfaces/Sharable.h"
#include "render/Buffer.h"

class CShader;

class CPostProcessRenderPass : public CSharable<CPostProcessRenderPass>,
                               public IEventsListener,
                               public IRenderPass
{
public:
  template <typename... Args>
  static SharedPtr Create(Args &&..._Args)
  {
    SharedPtr Instance = CSharable::Create(std::forward<Args>(_Args)...);
    Instance->SubscribeToEvents();
    return Instance;
  }

public:
  explicit CPostProcessRenderPass(std::shared_ptr<CShader> _Shader);

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
  float m_HDRExposure;
};