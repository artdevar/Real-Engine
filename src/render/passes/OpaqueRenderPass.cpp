#include "OpaqueRenderPass.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "interfaces/Renderer.h"
#include "assets/Texture.h"

COpaqueRenderPass::COpaqueRenderPass(std::shared_ptr<CShader> _Shader) :
    m_Shader(std::move(_Shader))
{
}

void COpaqueRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthFunc(GL_LESS);
  _Renderer.SetDepthMask(true);
  _Renderer.SetCullFace(ECullMode::Back);
  _Renderer.SetBlending(EAlphaMode::Opaque);

  _Renderer.SetShader(m_Shader);
  _Renderer.SetUniform("u_ViewPos", _RenderContext.CameraPosition);
  _Renderer.SetUniform("u_LightSpaceMatrix", _RenderContext.LightSpaceMatrix);
  _Renderer.SetUniform("u_ShadowMap", TEXTURE_SHADOW_MAP_INDEX);
  CTexture::Bind(TEXTURE_SHADOW_MAP_UNIT, _RenderContext.ShadowMap);
}

void COpaqueRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  for (const TRenderCommand &Command : _Commands)
  {
    _Renderer.SetUniform("u_Model", Command.ModelMatrix);
    _Renderer.SetUniform("u_MVP", _RenderContext.ViewProjectionMatrix * Command.ModelMatrix);

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
    _Renderer.SetBlending(Command.Material.AlphaMode);

    Command.VAO.get().Bind();

    if (Command.IndexType != EIndexType::None)
      _Renderer.DrawElements(Command.PrimitiveMode, Command.IndicesCount, Command.IndexType);
    else
      _Renderer.DrawArrays(Command.PrimitiveMode, Command.IndicesCount);

    Command.VAO.get().Unbind();
  }
}

void COpaqueRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
}

bool COpaqueRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.Material.SkyboxTexture == CCubemap::INVALID_VALUE && _Command.Material.AlphaMode != EAlphaMode::Blend;
}

bool COpaqueRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}