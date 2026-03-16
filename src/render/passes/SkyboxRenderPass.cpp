#include "SkyboxRenderPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "render/RenderTarget.h"
#include "render/ShaderTypes.h"
#include "render/Buffer.h"
#include "assets/Texture.h"
#include "utils/Resource.h"

CSkyboxRenderPass::CSkyboxRenderPass() :
    m_Shader(resource::LoadShader("Skybox"))
{
}

void CSkyboxRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthFunc(GL_LEQUAL);
  _Renderer.SetDepthMask(false);
  _Renderer.SetCullFace(ECullMode::None);
  _Renderer.SetViewport(_RenderContext.SceneRenderTarget.Size);
  _Renderer.SetShader(m_Shader);
  _Renderer.SetUniform("u_View", glm::mat4(glm::mat3(_RenderContext.ViewMatrix)));
  _Renderer.SetUniform("u_Projection", _RenderContext.ProjectionMatrix);
  _RenderContext.CubeVAO.Bind();
}

void CSkyboxRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  for (const TRenderCommand *Command : _Commands)
  {
    CCubemap::Bind(TEXTURE_SKYBOX_UNIT, Command->Environment.SkyboxTexture);
    _Renderer.SetUniform("u_Cubemap", TEXTURE_SKYBOX_INDEX);

    _Renderer.DrawArrays(EPrimitiveMode::Triangles, ARRAY_SIZE(CUBE_VERTICES) / 3);
  }
}

void CSkyboxRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _RenderContext.CubeVAO.Unbind();
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

bool CSkyboxRenderPass::NeedsCommands() const
{
  return true;
}