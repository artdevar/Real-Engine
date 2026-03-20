#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include "render/Buffer.h"
#include <events/EventsListener.h>
#include <common/Sharable.h>
#include <memory>

class CShader;

class CCollisionRenderPass final : public CSharable<CCollisionRenderPass>,
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
  explicit CCollisionRenderPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::Collision;
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
  glm::vec4                m_WireframeColor;
  CVertexArray             m_VAO;
  CVertexBuffer            m_VBO;
};
