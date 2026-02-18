#include "TransparentRenderPass.h"
#include "interfaces/Renderer.h"
#include "render/FrameContext.h"
#include "render/RenderCommand.h"
#include "assets/Texture.h"
#include "engine/Camera.h"

TransparentRenderPass::TransparentRenderPass(std::shared_ptr<CShader> _Shader) :
    m_Shader(std::move(_Shader))
{
}

void TransparentRenderPass::PreExecute(IRenderer &_Renderer, TFrameContext &_FrameContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthFunc(GL_LESS);
  _Renderer.SetDepthMask(false);
  _Renderer.SetCullFace(ECullMode::Back);
  _Renderer.SetBlending(EAlphaMode::Blend);

  _Renderer.SetShader(m_Shader.lock());
  _Renderer.SetUniform("u_ViewPos", _FrameContext.CameraPosition);
  _Renderer.SetUniform("u_LightSpaceMatrix", _FrameContext.LightSpaceMatrix);
  _Renderer.SetUniform("u_ShadowMap", TEXTURE_SHADOW_MAP_INDEX);
  CTexture::Bind(TEXTURE_SHADOW_MAP_UNIT, _FrameContext.ShadowMap);
}

void TransparentRenderPass::Execute(IRenderer &_Renderer, TFrameContext &_FrameContext, std::span<TRenderCommand> _Commands)
{
  for (const TRenderCommand &Command : _Commands)
  {
    _Renderer.SetUniform("u_Model", Command.ModelMatrix);
    _Renderer.SetUniform("u_MVP", _FrameContext.ViewProjectionMatrix * Command.ModelMatrix);

    CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, Command.Material.BaseColorTexture);
    CTexture::Bind(TEXTURE_NORMAL_UNIT, Command.Material.NormalTexture);
    CTexture::Bind(TEXTURE_EMISSIVE_UNIT, Command.Material.EmissiveTexture);
    CTexture::Bind(TEXTURE_METALLIC_ROUGHNESS_UNIT, Command.Material.MetallicRoughnessTexture);

    _Renderer.SetUniform("u_Material.BaseColorTexture", TEXTURE_BASIC_COLOR_INDEX);
    _Renderer.SetUniform("u_Material.NormalTexture", TEXTURE_NORMAL_INDEX);
    _Renderer.SetUniform("u_Material.EmissiveTexture", TEXTURE_EMISSIVE_INDEX);
    _Renderer.SetUniform("u_Material.MetallicRoughnessTexture", TEXTURE_METALLIC_ROUGHNESS_INDEX);
    _Renderer.SetUniform("u_Material.BaseColorTextureTexCoordIndex", Command.Material.BaseColorTextureTexCoordIndex);
    _Renderer.SetUniform("u_Material.NormalTextureTexCoordIndex", Command.Material.NormalTextureTexCoordIndex);
    _Renderer.SetUniform("u_Material.EmissiveTextureTexCoordIndex", Command.Material.EmissiveTextureTexCoordIndex);
    _Renderer.SetUniform("u_Material.MetallicRoughnessTextureTexCoordIndex", Command.Material.MetallicRoughnessTextureTexCoordIndex);
    _Renderer.SetUniform("u_Material.BaseColorFactor", Command.Material.BaseColorFactor);
    _Renderer.SetUniform("u_Material.EmissiveFactor", Command.Material.EmissiveFactor);
    _Renderer.SetUniform("u_Material.MetallicFactor", Command.Material.MetallicFactor);
    _Renderer.SetUniform("u_Material.RoughnessFactor", Command.Material.RoughnessFactor);
    _Renderer.SetUniform("u_Material.IsDoubleSided", Command.Material.IsDoubleSided);
    _Renderer.SetUniform("u_Material.AlphaMode", static_cast<int>(Command.Material.AlphaMode));
    _Renderer.SetUniform("u_Material.AlphaCutoff", Command.Material.AlphaCutoff);
    _Renderer.SetCullFace(Command.Material.IsDoubleSided ? ECullMode::None : ECullMode::Back);

    Command.VAO.get().Bind();

    if (Command.IndexType != EIndexType::None)
      _Renderer.DrawElements(Command.PrimitiveMode, Command.IndicesCount, Command.IndexType);
    else
      _Renderer.DrawArrays(Command.PrimitiveMode, Command.IndicesCount);

    Command.VAO.get().Unbind();
  }
}

void TransparentRenderPass::PostExecute(IRenderer &_Renderer, TFrameContext &_FrameContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthMask(true); // Required for clearing the depth buffer
}

bool TransparentRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.Material.AlphaMode == EAlphaMode::Blend;
}

bool TransparentRenderPass::IsAvailable() const
{
  return !m_Shader.expired();
}