#include "PostProcessRenderPass.h"
#include "render/RenderCommand.h"
#include "render/RenderContext.h"
#include "render/RenderTarget.h"
#include "interfaces/Renderer.h"
#include "utils/Event.h"
#include "assets/Texture.h"
#include "engine/Config.h"
#include "utils/Resource.h"

CPostProcessRenderPass::CPostProcessRenderPass() :
    m_Shader(resource::LoadShader("PostProcess")),
    m_IsFXAAEnabled(CConfig::Instance().GetFXAAEnabled()),
    m_IsHDREnabled(CConfig::Instance().GetHDREnabled()),
    m_IsBloomEnabled(CConfig::Instance().GetBloomEnabled()),
    m_IsGammaCorrectionEnabled(CConfig::Instance().GetGammaCorrectionEnabled()),
    m_HDRExposure(CConfig::Instance().GetHDRExposure()),
    m_BloomIntensity(CConfig::Instance().GetBloomIntensity()),
    m_Gamma(CConfig::Instance().GetGamma())
{
}

void CPostProcessRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _RenderContext.PostProcessRenderTarget.FrameBuffer.Bind();
  _Renderer.SetDepthTest(false);
  _Renderer.SetViewport(_RenderContext.PostProcessRenderTarget.Size);
  _Renderer.SetShader(m_Shader);
  _RenderContext.QuadVAO.Bind();
}

void CPostProcessRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  const glm::vec2 InverseSize = glm::vec2(1.0f / _RenderContext.PostProcessRenderTarget.Size.X, 1.0f / _RenderContext.PostProcessRenderTarget.Size.Y);

  CTexture::Bind(TEXTURE_DEPTH_MAP_UNIT, std::get<TRenderTarget::TTexture>(_RenderContext.SceneRenderTarget.Depth)->ID());
  CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _RenderContext.SceneRenderTarget.Color->ID());
  CTexture::Bind(TEXTURE_BLOOM_UNIT, _RenderContext.BloomMap);
  CTexture::Bind(TEXTURE_TAA_HISTORY_UNIT, _RenderContext.TAAHistoryMap);

  _Renderer.SetUniform("ColorTexture", TEXTURE_BASIC_COLOR_INDEX);
  _Renderer.SetUniform("DepthTexture", TEXTURE_DEPTH_MAP_INDEX);
  _Renderer.SetUniform("BloomTexture", TEXTURE_BLOOM_INDEX);
  _Renderer.SetUniform("TAATexture", TEXTURE_TAA_HISTORY_INDEX);
  _Renderer.SetUniform("InverseScreenSize", InverseSize);
  _Renderer.SetUniform("IsFXAAEnabled", m_IsFXAAEnabled);
  _Renderer.SetUniform("IsTAAEnabled", m_IsTAAEnabled);
  _Renderer.SetUniform("IsHDR", m_IsHDREnabled);
  _Renderer.SetUniform("IsBloomEnabled", m_IsBloomEnabled);
  _Renderer.SetUniform("BloomIntensity", m_BloomIntensity);
  _Renderer.SetUniform("IsGammaCorrectionEnabled", m_IsGammaCorrectionEnabled);
  _Renderer.SetUniform("Exposure", m_HDRExposure);
  _Renderer.SetUniform("Gamma", m_Gamma);
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
}

void CPostProcessRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _RenderContext.QuadVAO.Unbind();
  _RenderContext.PostProcessRenderTarget.FrameBuffer.Unbind();
}

bool CPostProcessRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return false;
}

bool CPostProcessRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}

bool CPostProcessRenderPass::NeedsCommands() const
{
  return false;
}

void CPostProcessRenderPass::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::Config_FXAAEnabledChanged:
    m_IsFXAAEnabled = _Event.GetValue<bool>();
    break;
  case TEventType::Config_TAAEnabledChanged:
    m_IsTAAEnabled = _Event.GetValue<bool>();
    break;
  case TEventType::Config_HDREnabledChanged:
    m_IsHDREnabled = _Event.GetValue<bool>();
    break;
  case TEventType::Config_HDRExposureChanged:
    m_HDRExposure = _Event.GetValue<float>();
    break;
  case TEventType::Config_BloomEnabledChanged:
    m_IsBloomEnabled = _Event.GetValue<bool>();
    break;
  case TEventType::Config_BloomIntensityChanged:
    m_BloomIntensity = _Event.GetValue<float>();
    break;
  case TEventType::Config_GammaCorrectionEnabledChanged:
    m_IsGammaCorrectionEnabled = _Event.GetValue<bool>();
    break;
  case TEventType::Config_GammaChanged:
    m_Gamma = _Event.GetValue<float>();
    break;

  default:
    break;
  }
}

void CPostProcessRenderPass::SubscribeToEvents()
{
  event::Subscribe(TEventType::Config_FXAAEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_TAAEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_HDREnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_HDRExposureChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_GammaCorrectionEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_GammaChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_BloomEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_BloomIntensityChanged, GetWeakPtr());
}
