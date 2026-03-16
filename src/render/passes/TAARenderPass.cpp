#include "TAARenderPass.h"
#include "render/RenderCommand.h"
#include "render/RenderContext.h"
#include "render/RenderTarget.h"
#include "interfaces/Renderer.h"
#include "utils/Event.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Config.h"
#include "utils/Resource.h"
#include <common/MathTypes.h>
#include <glm/glm.hpp>

CTAARenderPass::CTAARenderPass(TVector2i _Viewport) :
    m_Shader(resource::LoadShader("TAA")),
    m_HistoryIndex(0)
{
  InitHistoryTargets(_Viewport);
}

void CTAARenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands)
{
  TRenderTarget &CurrentOutput = *m_HistoryTargets[m_HistoryIndex];
  CurrentOutput.FrameBuffer.Bind();

  _Renderer.SetDepthTest(false);
  _Renderer.SetViewport(_RenderContext.SceneRenderTarget.Size);
  _Renderer.SetShader(m_Shader);

  _RenderContext.QuadVAO.Bind();
}

void CTAARenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands)
{
  TRenderTarget &PrevTarget = *m_HistoryTargets[!m_HistoryIndex];

  const glm::vec2 InverseSize = glm::vec2(1.0f / PrevTarget.Size.X, 1.0f / PrevTarget.Size.Y);

  CTexture::Bind(TEXTURE_TAA_HISTORY_UNIT, PrevTarget.Color->ID());
  CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _RenderContext.SceneRenderTarget.Color->ID());
  CTexture::Bind(TEXTURE_VELOCITY_UNIT, _RenderContext.SceneRenderTarget.Velocity->ID());
  CTexture::Bind(TEXTURE_DEPTH_MAP_UNIT, std::get<TRenderTarget::TTexture>(_RenderContext.SceneRenderTarget.Depth)->ID());
  _Renderer.SetUniform("CurrentFrame", TEXTURE_BASIC_COLOR_INDEX);
  _Renderer.SetUniform("HistoryFrame", TEXTURE_TAA_HISTORY_INDEX);
  _Renderer.SetUniform("VelocityTex", TEXTURE_VELOCITY_INDEX);
  _Renderer.SetUniform("DepthTexture", TEXTURE_DEPTH_MAP_INDEX);
  _Renderer.SetUniform("CurrentViewProjection", _RenderContext.JitteredViewProjectionMatrix);
  _Renderer.SetUniform("PreviousViewProjection", _RenderContext.PreviousViewProjectionMatrix);
  _Renderer.SetUniform("Jitter", _RenderContext.Jitter);
  _Renderer.SetUniform("PrevJitter", _RenderContext.PrevJitter);
  _Renderer.SetUniform("InverseScreenSize", InverseSize);

  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
}

void CTAARenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands)
{
  _RenderContext.QuadVAO.Unbind();

  _RenderContext.TAAHistoryMap = m_HistoryTargets[m_HistoryIndex]->Color->ID();
  m_HistoryIndex               = !m_HistoryIndex;
}

bool CTAARenderPass::Accepts(const TRenderCommand &_Command) const
{
  return false;
}

bool CTAARenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}

bool CTAARenderPass::NeedsCommands() const
{
  return false;
}

void CTAARenderPass::InitHistoryTargets(const TVector2i &_Viewport)
{
  const auto CreateDepthRBO = [](TVector2i _Size) {
    CRenderBuffer DepthRBO;
    DepthRBO.Bind();
    DepthRBO.AllocateStorage(GL_DEPTH_COMPONENT, _Size.X, _Size.Y);
    DepthRBO.Unbind();
    return DepthRBO;
  };

  TTextureParams TextureParams{
      .Width          = _Viewport.X,
      .Height         = _Viewport.Y,
      .InternalFormat = GL_RGBA16F,
      .Format         = GL_RGBA,
      .Type           = GL_FLOAT,
      .MinFilter      = ETextureFilter::Linear,
      .MagFilter      = ETextureFilter::Linear,
  };

  m_HistoryTargets[0]        = std::make_unique<TRenderTarget>();
  m_HistoryTargets[0]->Size  = _Viewport;
  m_HistoryTargets[0]->Depth = CreateDepthRBO(_Viewport);
  m_HistoryTargets[0]->Color = resource::RecreateTexture("TAA_HISTORY_A", TextureParams);
  m_HistoryTargets[0]->FrameBuffer.Bind();
  m_HistoryTargets[0]->FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, m_HistoryTargets[0]->Color->ID());
  m_HistoryTargets[0]->FrameBuffer.AttachRenderBuffer(GL_DEPTH_ATTACHMENT, std::get<CRenderBuffer>(m_HistoryTargets[0]->Depth).ID());
  m_HistoryTargets[0]->FrameBuffer.Unbind();

  m_HistoryTargets[1]        = std::make_unique<TRenderTarget>();
  m_HistoryTargets[1]->Size  = _Viewport;
  m_HistoryTargets[1]->Depth = CreateDepthRBO(_Viewport);
  m_HistoryTargets[1]->Color = resource::RecreateTexture("TAA_HISTORY_B", TextureParams);
  m_HistoryTargets[1]->FrameBuffer.Bind();
  m_HistoryTargets[1]->FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, m_HistoryTargets[1]->Color->ID());
  m_HistoryTargets[1]->FrameBuffer.AttachRenderBuffer(GL_DEPTH_ATTACHMENT, std::get<CRenderBuffer>(m_HistoryTargets[1]->Depth).ID());
  m_HistoryTargets[1]->FrameBuffer.Unbind();
}

void CTAARenderPass::SubscribeToEvents()
{
  event::Subscribe(TEventType::ViewportResized, GetWeakPtr());
}

void CTAARenderPass::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::ViewportResized: {
    const TVector2i NewViewport = _Event.GetValue<TVector2i>();
    InitHistoryTargets(NewViewport);
    break;
  }
  default:
    break;
  }
}
