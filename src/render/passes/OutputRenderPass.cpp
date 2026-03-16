#include "OutputRenderPass.h"

#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderTarget.h"
#include "render/RenderCommand.h"
#include "render/ShaderTypes.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Engine.h"
#include "utils/Resource.h"

COutputRenderPass::COutputRenderPass() :
    m_Shader(resource::LoadShader("Output"))
{
}

void COutputRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthTest(false);
  _Renderer.SetBlending(EAlphaMode::Opaque);
  _Renderer.SetCullFace(ECullMode::None);
  _Renderer.SetViewport(_RenderContext.PostProcessRenderTarget.Size);
  _Renderer.SetShader(m_Shader);
  _RenderContext.QuadVAO.Bind();
}

void COutputRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _RenderContext.PostProcessRenderTarget.Color->ID());
  _Renderer.SetUniform("Texture", TEXTURE_BASIC_COLOR_INDEX);
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
}

void COutputRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _RenderContext.QuadVAO.Unbind();
}

bool COutputRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return false;
}

bool COutputRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}

bool COutputRenderPass::NeedsCommands() const
{
  return false;
}
