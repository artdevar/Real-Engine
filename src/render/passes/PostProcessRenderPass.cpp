#include "PostProcessRenderPass.h"
#include "render/RenderCommand.h"
#include "render/RenderContext.h"
#include "interfaces/Renderer.h"
#include "utils/Event.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Config.h"

static constexpr float QUAD_VERTICES[] = {
    // positions        // texcoords

    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //

    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
};

CPostProcessRenderPass::CPostProcessRenderPass(std::shared_ptr<CShader> _Shader) :
    m_Shader(std::move(_Shader)),
    m_VAO(),
    m_VBO(GL_STATIC_DRAW),
    m_IsFxaaEnabled(CConfig::Instance().GetFXAAEnabled()),
    m_IsHDREnabled(CConfig::Instance().GetHDREnabled()),
    m_HDRExposure(CConfig::Instance().GetHDRExposure())
{
  m_VAO.Bind();
  m_VBO.Bind();
  m_VBO.Assign(QUAD_VERTICES, sizeof(QUAD_VERTICES));
  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 5 * sizeof(float), (void *)0);
  m_VAO.EnableAttrib(ATTRIB_LOC_TEXCOORDS_0, 2, GL_FLOAT, false, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  m_VAO.Unbind();
}

void CPostProcessRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetShader(m_Shader);
  m_VAO.Bind();
}

void CPostProcessRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _RenderContext.RenderTexture);
  _Renderer.SetUniform("Texture", TEXTURE_BASIC_COLOR_INDEX);
  _Renderer.SetUniform("InverseScreenSize", glm::vec2(1.0f / _Renderer.GetViewport().X, 1.0f / _Renderer.GetViewport().Y));
  _Renderer.SetUniform("IsFXAAEnabled", m_IsFxaaEnabled);
  _Renderer.SetUniform("IsHDR", m_IsHDREnabled);
  _Renderer.SetUniform("Exposure", m_HDRExposure);
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
}

void CPostProcessRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  m_VAO.Unbind();
}

bool CPostProcessRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return true;
}

bool CPostProcessRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
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

  default:
    break;
  }
}

void CPostProcessRenderPass::SubscribeToEvents()
{
  event::Subscribe(TEventType::Config_FXAAEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_HDREnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_HDRExposureChanged, GetWeakPtr());
}