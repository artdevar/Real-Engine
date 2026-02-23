#include "PostProcessRenderPass.h"
#include "render/RenderCommand.h"
#include "render/RenderContext.h"
#include "interfaces/Renderer.h"
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

PostProcessRenderPass::PostProcessRenderPass(std::shared_ptr<CShader> _Shader) :
    m_Shader(std::move(_Shader)),
    m_VAO(),
    m_VBO(GL_STATIC_DRAW)
{
  m_VAO.Bind();
  m_VBO.Bind();
  m_VBO.Assign(QUAD_VERTICES, sizeof(QUAD_VERTICES));
  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 5 * sizeof(float), (void *)0);
  m_VAO.EnableAttrib(ATTRIB_LOC_TEXCOORDS_0, 2, GL_FLOAT, false, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  m_VAO.Unbind();
}

void PostProcessRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetShader(m_Shader.lock());
  m_VAO.Bind();
}

void PostProcessRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _RenderContext.RenderTexture);
  _Renderer.SetUniform("Texture", TEXTURE_BASIC_COLOR_INDEX);
  _Renderer.SetUniform("InverseScreenSize", glm::vec2(1.0f / _Renderer.GetViewport().X, 1.0f / _Renderer.GetViewport().Y));
  _Renderer.SetUniform("IsFXAAEnabled", CConfig::Instance().GetFXAAEnabled());
  _Renderer.SetUniform("IsHDR", CConfig::Instance().GetHDREnabled());
  _Renderer.SetUniform("Exposure", CConfig::Instance().GetHDRExposure());
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
}

void PostProcessRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  m_VAO.Unbind();
}

bool PostProcessRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return true;
}

bool PostProcessRenderPass::IsAvailable() const
{
  return !m_Shader.expired();
}