#include "PostProcessRenderPass.h"
#include "render/RenderCommand.h"
#include "render/RenderContext.h"
#include "render/RenderTarget.h"
#include "interfaces/Renderer.h"
#include "utils/Event.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Config.h"
#include "utils/Resource.h"

CPostProcessRenderPass::CPostProcessRenderPass() :
    m_Shader(resource::LoadShader("PostProcess")),
    m_VAO(),
    m_VBO(GL_STATIC_DRAW),
    m_IsFxaaEnabled(CConfig::Instance().GetFXAAEnabled()),
    m_IsHDREnabled(CConfig::Instance().GetHDREnabled()),
    m_IsGammaCorrectionEnabled(CConfig::Instance().GetGammaCorrectionEnabled()),
    m_HDRExposure(CConfig::Instance().GetHDRExposure()),
    m_Gamma(CConfig::Instance().GetGamma())
{
  m_VAO.Bind();
  m_VBO.Bind();
  m_VBO.Assign(QUAD_VERTICES, sizeof(QUAD_VERTICES));
  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 5 * sizeof(float), (void *)0);
  m_VAO.EnableAttrib(ATTRIB_LOC_TEXCOORDS_0, 2, GL_FLOAT, false, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  m_VAO.Unbind();
}

void CPostProcessRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthTest(false);
  _Renderer.SetViewport(_RenderContext.PostProcessRenderTarget.Size);
  _Renderer.SetShader(m_Shader);
  m_VAO.Bind();
}

void CPostProcessRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _RenderContext.SceneRenderTarget.Color->ID());
  _Renderer.SetUniform("Texture", TEXTURE_BASIC_COLOR_INDEX);
  _Renderer.SetUniform("InverseScreenSize", glm::vec2(1.0f / _Renderer.GetViewport().X, 1.0f / _Renderer.GetViewport().Y));
  _Renderer.SetUniform("IsFXAAEnabled", m_IsFxaaEnabled);
  _Renderer.SetUniform("IsHDR", m_IsHDREnabled);
  _Renderer.SetUniform("IsGammaCorrectionEnabled", m_IsGammaCorrectionEnabled);
  _Renderer.SetUniform("Exposure", m_HDRExposure);
  _Renderer.SetUniform("Gamma", m_Gamma);
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
}

void CPostProcessRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  m_VAO.Unbind();
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
    m_IsFxaaEnabled = _Event.GetValue<bool>();
    break;
  case TEventType::Config_HDREnabledChanged:
    m_IsHDREnabled = _Event.GetValue<bool>();
    break;
  case TEventType::Config_HDRExposureChanged:
    m_HDRExposure = _Event.GetValue<float>();
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
  event::Subscribe(TEventType::Config_HDREnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_HDRExposureChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_GammaCorrectionEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_GammaChanged, GetWeakPtr());
}