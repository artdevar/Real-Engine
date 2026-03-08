#include "SkyboxRenderPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "render/RenderTarget.h"
#include "render/ShaderTypes.h"
#include "render/Buffer.h"
#include "assets/Texture.h"
#include "utils/Resource.h"

static constexpr float SKYBOX_VERTICES[] = {
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  1.0f,  -1.0f, //
    -1.0f, 1.0f,  -1.0f, //

    -1.0f, -1.0f, 1.0f,  //
    -1.0f, -1.0f, -1.0f, //
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, 1.0f,  1.0f,  //
    -1.0f, -1.0f, 1.0f,  //

    1.0f,  -1.0f, -1.0f, //
    1.0f,  -1.0f, 1.0f,  //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  -1.0f, //
    1.0f,  -1.0f, -1.0f, //

    -1.0f, -1.0f, 1.0f, //
    -1.0f, 1.0f,  1.0f, //
    1.0f,  1.0f,  1.0f, //
    1.0f,  1.0f,  1.0f, //
    1.0f,  -1.0f, 1.0f, //
    -1.0f, -1.0f, 1.0f, //

    -1.0f, 1.0f,  -1.0f, //
    1.0f,  1.0f,  -1.0f, //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  1.0f,  //
    -1.0f, 1.0f,  1.0f,  //
    -1.0f, 1.0f,  -1.0f, //

    -1.0f, -1.0f, -1.0f, //
    -1.0f, -1.0f, 1.0f,  //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    -1.0f, -1.0f, 1.0f,  //
    1.0f,  -1.0f, 1.0f   //
};

CSkyboxRenderPass::CSkyboxRenderPass() :
    m_Shader(resource::LoadShader("Skybox")),
    m_VAO(),
    m_VBO(GL_STATIC_DRAW)
{
  m_VAO.Bind();
  m_VBO.Bind();
  m_VBO.Assign(SKYBOX_VERTICES, sizeof(SKYBOX_VERTICES));

  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 3 * sizeof(float));
  m_VAO.Unbind();
  m_VBO.Unbind();
}

void CSkyboxRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthFunc(GL_LEQUAL);
  _Renderer.SetDepthMask(false);
  _Renderer.SetCullFace(ECullMode::None);
  _Renderer.SetViewport(_RenderContext.SceneRenderTarget.Size);
  _Renderer.SetShader(m_Shader);
  _Renderer.SetUniform("u_View", glm::mat4(glm::mat3(_RenderContext.ViewMatrix)));
  _Renderer.SetUniform("u_Projection", _RenderContext.ProjectionMatrix);
}

void CSkyboxRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  m_VAO.Bind();

  for (const TRenderCommand &Command : _Commands)
  {
    CCubemap::Bind(TEXTURE_SKYBOX_UNIT, Command.Material.SkyboxTexture);
    _Renderer.SetUniform("u_Cubemap", TEXTURE_SKYBOX_INDEX);

    _Renderer.DrawArrays(EPrimitiveMode::Triangles, ARRAY_SIZE(SKYBOX_VERTICES) / 3);
  }

  m_VAO.Unbind();
}

void CSkyboxRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthMask(true);
  _Renderer.SetDepthFunc(GL_LESS);
}

bool CSkyboxRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.RenderFlags.test(ERenderFlags_Skybox);
}

bool CSkyboxRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}