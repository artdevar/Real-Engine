#include "SkyboxRenderPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "render/ShaderTypes.h"
#include "assets/Texture.h"

CSkyboxRenderPass::CSkyboxRenderPass(std::shared_ptr<CShader> _Shader) :
    m_Shader(std::move(_Shader))
{
}

void CSkyboxRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthFunc(GL_LEQUAL);
  _Renderer.SetDepthMask(false);
  _Renderer.SetCullFace(ECullMode::None);

  _Renderer.SetShader(m_Shader);
  _Renderer.SetUniform("u_View", glm::mat4(glm::mat3(_RenderContext.ViewMatrix)));
  _Renderer.SetUniform("u_Projection", _RenderContext.ProjectionMatrix);
}

void CSkyboxRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  for (const TRenderCommand &Command : _Commands)
  {
    CCubemap::Bind(TEXTURE_SKYBOX_UNIT, Command.Material.SkyboxTexture);
    _Renderer.SetUniform("u_Cubemap", TEXTURE_SKYBOX_INDEX);

    Command.VAO.get().Bind();

    if (Command.IndexType != EIndexType::None)
      _Renderer.DrawElements(Command.PrimitiveMode, Command.IndicesCount, Command.IndexType);
    else
      _Renderer.DrawArrays(Command.PrimitiveMode, Command.IndicesCount);

    Command.VAO.get().Unbind();
  }
}

void CSkyboxRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthMask(true);
  _Renderer.SetDepthFunc(GL_LESS);
}

bool CSkyboxRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.Material.SkyboxTexture != CCubemap::INVALID_VALUE;
}

bool CSkyboxRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}