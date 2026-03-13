#include "CollisionRenderPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "render/Buffer.h"
#include "engine/Config.h"
#include "assets/Shader.h"
#include "utils/Resource.h"
#include "utils/Event.h"
#include <common/GlmUtils.h>

static constexpr float WIREFRAME_VERTICES[] = {
    // Bottom square
    -0.5f, -0.5f, -0.5f, //
    0.5f, -0.5f, -0.5f,  //
    0.5f, -0.5f, -0.5f,  //
    0.5f, -0.5f, 0.5f,   //
    0.5f, -0.5f, 0.5f,   //
    -0.5f, -0.5f, 0.5f,  //
    -0.5f, -0.5f, 0.5f,  //
    -0.5f, -0.5f, -0.5f, //

    // Top square
    -0.5f, 0.5f, -0.5f, //
    0.5f, 0.5f, -0.5f,  //
    0.5f, 0.5f, -0.5f,  //
    0.5f, 0.5f, 0.5f,   //
    0.5f, 0.5f, 0.5f,   //
    -0.5f, 0.5f, 0.5f,  //
    -0.5f, 0.5f, 0.5f,  //
    -0.5f, 0.5f, -0.5f, //

    // Vertical lines
    -0.5f, -0.5f, -0.5f, //
    -0.5f, 0.5f, -0.5f,  //
    0.5f, -0.5f, -0.5f,  //
    0.5f, 0.5f, -0.5f,   //
    0.5f, -0.5f, 0.5f,   //
    0.5f, 0.5f, 0.5f,    //
    -0.5f, -0.5f, 0.5f,  //
    -0.5f, 0.5f, 0.5f};

CCollisionRenderPass::CCollisionRenderPass() :
    m_Shader(resource::LoadShader("Wireframe")),
    m_WireframeColor(glm::make_vec4(CConfig::Instance().GetWireframeColor())),
    m_VAO(),
    m_VBO(GL_STATIC_DRAW)
{
  m_VAO.Bind();
  m_VBO.Bind();
  m_VBO.Assign(WIREFRAME_VERTICES, sizeof(WIREFRAME_VERTICES));

  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));

  m_VAO.Unbind();
}

void CCollisionRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthTest(false);
  _Renderer.SetCullFace(ECullMode::None);
  _Renderer.SetShader(m_Shader);
}

void CCollisionRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  m_VAO.Bind();

  for (const TRenderCommand *Command : _Commands)
  {
    _Renderer.SetUniform("u_MVP", _RenderContext.ViewProjectionMatrix * Command->ModelMatrix);
    _Renderer.SetUniform("u_Color", m_WireframeColor);

    _Renderer.DrawArrays(EPrimitiveMode::Lines, ARRAY_SIZE(WIREFRAME_VERTICES));
  }

  m_VAO.Unbind();
}

void CCollisionRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
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

bool CCollisionRenderPass::NeedsCommands() const
{
  return true;
}

void CCollisionRenderPass::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::Config_WireframeColorChanged:
    const TColor WireframeColor = _Event.GetValue<TColor>();
    m_WireframeColor            = glm::make_vec4(WireframeColor);
    break;
  }
}

void CCollisionRenderPass::SubscribeToEvents()
{
  event::Subscribe(TEventType::Config_WireframeColorChanged, GetWeakPtr());
}
