#include "CollisionRenderPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "engine/Config.h"
#include "assets/Shader.h"
#include "utils/Resource.h"
#include "utils/Event.h"
#include <common/GlmUtils.h>

CCollisionRenderPass::CCollisionRenderPass() :
    m_Shader(resource::LoadShader("Wireframe")),
    m_WireframeColor(glm::ToVec4(CConfig::Instance().GetWireframeColor()))
{
}

void CCollisionRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthTest(false);
  _Renderer.SetCullFace(ECullMode::None);
  _Renderer.SetShader(m_Shader);
}

void CCollisionRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  for (const TRenderCommand &Command : _Commands)
  {
    _Renderer.SetUniform("u_MVP", _RenderContext.ViewProjectionMatrix * Command.ModelMatrix);
    _Renderer.SetUniform("u_Color", m_WireframeColor);

    Command.VAO.get().Bind();
    _Renderer.DrawArrays(Command.PrimitiveMode, Command.IndicesCount);
    Command.VAO.get().Unbind();
  }
}

void CCollisionRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
}

bool CCollisionRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.RenderFlags.test(ERenderFlags_Wireframe);
}

bool CCollisionRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}

void CCollisionRenderPass::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::Config_WireframeColorChanged:
    const TColor WireframeColor = _Event.GetValue<TColor>();
    m_WireframeColor            = glm::ToVec4(WireframeColor);
    break;
  }
}

void CCollisionRenderPass::SubscribeToEvents()
{
  event::Subscribe(TEventType::Config_WireframeColorChanged, GetWeakPtr());
}
