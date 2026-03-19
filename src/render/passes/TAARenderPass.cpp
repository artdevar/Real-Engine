#include "TAARenderPass.h"
#include "render/RenderCommand.h"
#include "render/RenderContext.h"
#include "render/RenderTarget.h"
#include "interfaces/Renderer.h"
#include "utils/Event.h"
#include "assets/Texture.h"
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
  _Renderer.SetViewport(CurrentOutput.Size);
  _Renderer.SetShader(m_Shader);

  _RenderContext.QuadVAO.Bind();
}

void CTAARenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands)
{
  TRenderTarget &PrevOutput = *m_HistoryTargets[!m_HistoryIndex];

  const glm::vec2 InverseSize = glm::vec2(1.0f / PrevOutput.Size.X, 1.0f / PrevOutput.Size.Y);

  C2DTexture::Bind(TEXTURE_TAA_HISTORY_UNIT, PrevOutput.Color->ID());
  C2DTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _RenderContext.SceneRenderTarget.Color->ID());
  C2DTexture::Bind(TEXTURE_VELOCITY_UNIT, _RenderContext.SceneRenderTarget.Velocity->ID());
  C2DTexture::Bind(TEXTURE_DEPTH_MAP_UNIT, std::get<TRenderTarget::TTexture>(_RenderContext.SceneRenderTarget.Depth)->ID());
  _Renderer.SetUniform("CurrentFrame", TEXTURE_BASIC_COLOR_INDEX);
  _Renderer.SetUniform("HistoryFrame", TEXTURE_TAA_HISTORY_INDEX);
  _Renderer.SetUniform("VelocityTexture", TEXTURE_VELOCITY_INDEX);
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
      .InternalFormat = EInternalFormat::RGBA16F,
      .Format         = EFormat::RGBA,
      .Type           = EType::Float,
      .MinFilter      = ETextureFilter::Linear,
      .MagFilter      = ETextureFilter::Linear,

  };

  for (int i = 0; i < HISTORY_TARGETS_COUNT; ++i)
  {
    m_HistoryTargets[i]        = std::make_unique<TRenderTarget>();
    m_HistoryTargets[i]->Size  = _Viewport;
    m_HistoryTargets[i]->Depth = CreateDepthRBO(_Viewport);
    m_HistoryTargets[i]->Color = resource::RecreateTexture("TAA_HISTORY" + std::to_string(i), TextureParams);
    m_HistoryTargets[i]->FrameBuffer.Bind();
    m_HistoryTargets[i]->FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, m_HistoryTargets[i]->Color->ID());
    m_HistoryTargets[i]->FrameBuffer.AttachRenderBuffer(GL_DEPTH_ATTACHMENT, std::get<CRenderBuffer>(m_HistoryTargets[i]->Depth).ID());
    m_HistoryTargets[i]->FrameBuffer.Unbind();
  }
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
