#include "BloomRenderPass.h"
#include "render/RenderCommand.h"
#include "render/RenderContext.h"
#include "render/RenderTarget.h"
#include "render/ShaderTypes.h"
#include "interfaces/Renderer.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Config.h"
#include "utils/Resource.h"

float CBloomRenderPass::m_Threshold  = 1.0f;
int   CBloomRenderPass::m_BlurPasses = 10;

CBloomRenderPass::CBloomRenderPass(TVector2i _Viewport) :
    m_DownsampleShader(resource::LoadShader("BloomDownsample")),
    m_BlurShader(resource::LoadShader("BloomBlur"))
{
  m_Threshold  = CConfig::Instance().GetBloomThreshold();
  m_BlurPasses = CConfig::Instance().GetBloomBlurPasses();

  InitTextures(_Viewport);
}

void CBloomRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthTest(false);
  _RenderContext.QuadVAO.Bind();
}

void CBloomRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  m_BloomFBO.Bind();
  _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color));
  _Renderer.SetViewport(m_BloomColor->GetSize());
  _Renderer.SetShader(m_DownsampleShader);
  CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _RenderContext.SceneRenderTarget.Color->ID());
  _Renderer.SetUniform("ColorTexture", TEXTURE_BASIC_COLOR_INDEX);
  _Renderer.SetUniform("Threshold", m_Threshold);
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
  m_BloomFBO.Unbind();

  _Renderer.SetShader(m_BlurShader);
  bool Horizontal = true;
  for (int i = 0; i < m_BlurPasses; i++)
  {
    m_PingPongFBO[Horizontal].Bind();

    CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, i == 0 ? m_BloomColor->ID() : m_PingPongColor[!Horizontal]->ID());
    _Renderer.SetUniform("Image", TEXTURE_BASIC_COLOR_INDEX);
    _Renderer.SetUniform("Horizontal", Horizontal);

    _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);

    Horizontal = !Horizontal;
  }
  _RenderContext.BloomMap = m_PingPongColor[!Horizontal]->ID();
}

void CBloomRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _RenderContext.QuadVAO.Unbind();
  CFrameBuffer::BindDefault();
}

bool CBloomRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return false;
}

bool CBloomRenderPass::IsAvailable() const
{
  return m_DownsampleShader != nullptr && m_BlurShader != nullptr;
}

bool CBloomRenderPass::NeedsCommands() const
{
  return false;
}

void CBloomRenderPass::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::ViewportResized: {
    const TVector2i NewViewport = _Event.GetValue<TVector2i>();
    InitTextures(NewViewport);
    break;
  }
  case TEventType::Config_BloomThresholdChanged:
    m_Threshold = _Event.GetValue<float>();
    break;
  case TEventType::Config_BloomBlurPassesChanged:
    m_BlurPasses = _Event.GetValue<int>();
    break;
  default:
    break;
  }
}

void CBloomRenderPass::SubscribeToEvents()
{
  event::Subscribe(TEventType::Config_BloomThresholdChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_BloomBlurPassesChanged, GetWeakPtr());
  event::Subscribe(TEventType::ViewportResized, GetWeakPtr());
}

void CBloomRenderPass::InitTextures(TVector2i _Size)
{
  TTextureParams Params;
  Params.Width          = _Size.X / 2;
  Params.Height         = _Size.Y / 2;
  Params.InternalFormat = GL_RGBA16F;
  Params.Format         = GL_RGBA;
  Params.Type           = GL_FLOAT;
  Params.MinFilter      = ETextureFilter::Linear;
  Params.MagFilter      = ETextureFilter::Linear;
  Params.WrapS          = ETextureWrap::ClampToEdge;
  Params.WrapT          = ETextureWrap::ClampToEdge;

  for (int i = 0; i < 2; ++i)
  {
    m_PingPongFBO[i].Bind();
    m_PingPongColor[i] = resource::RecreateTexture("BLOOM_COLOR_" + std::to_string(i + 1), Params);
    m_PingPongFBO[i].AttachTexture(GL_COLOR_ATTACHMENT0, m_PingPongColor[i]->ID());
  }

  m_BloomFBO.Bind();
  m_BloomColor = resource::RecreateTexture("BLOOM_COLOR", Params);
  m_BloomFBO.AttachTexture(GL_COLOR_ATTACHMENT0, m_BloomColor->ID());
}
