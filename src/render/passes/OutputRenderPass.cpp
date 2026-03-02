#include "OutputRenderPass.h"

#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderTarget.h"
#include "render/RenderCommand.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Engine.h"

static constexpr float FULLSCREEN_QUAD_VERTICES[] = {
    // positions        // texcoords

    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //

    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
};

COutputRenderPass::COutputRenderPass(std::shared_ptr<CShader> _Shader) :
    m_Shader(std::move(_Shader)),
    m_VAO(),
    m_VBO(GL_STATIC_DRAW)
{
  m_VAO.Bind();
  m_VBO.Bind();
  m_VBO.Assign(FULLSCREEN_QUAD_VERTICES, sizeof(FULLSCREEN_QUAD_VERTICES));
  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 5 * sizeof(float), (void *)0);
  m_VAO.EnableAttrib(ATTRIB_LOC_TEXCOORDS_0, 2, GL_FLOAT, false, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  m_VAO.Unbind();
}

void COutputRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthTest(false);
  _Renderer.SetBlending(EAlphaMode::Opaque);
  _Renderer.SetCullFace(ECullMode::None);
  _Renderer.SetViewport(_RenderContext.PostProcessRenderTarget.Size);
  _Renderer.SetShader(m_Shader);
  m_VAO.Bind();
}

void COutputRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _RenderContext.PostProcessRenderTarget.Color->ID());
  _Renderer.SetUniform("Texture", TEXTURE_BASIC_COLOR_INDEX);
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
}

void COutputRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  m_VAO.Unbind();
}

bool COutputRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return true;
}

bool COutputRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}
