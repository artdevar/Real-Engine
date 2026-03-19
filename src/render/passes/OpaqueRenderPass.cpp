#include "OpaqueRenderPass.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "render/RenderTarget.h"
#include "interfaces/Renderer.h"
#include "assets/Texture.h"
#include "utils/Resource.h"

COpaqueRenderPass::COpaqueRenderPass() :
    m_Shader(resource::LoadShader("PBR"))
{
}

void COpaqueRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthMask(true);
  _Renderer.SetDepthFunc(GL_LESS);
  _Renderer.SetCullFace(ECullMode::Back);
  _Renderer.SetBlending(EAlphaMode::Opaque);
  _Renderer.SetViewport(_RenderContext.SceneRenderTarget.Size);
  _Renderer.SetShader(m_Shader);
  _Renderer.SetUniform("u_ViewPos", _RenderContext.CameraPosition);
  _Renderer.SetUniform("u_LightSpaceMatrix", _RenderContext.LightSpaceMatrix);
  _Renderer.SetUniform("u_JitteredCurrVP", _RenderContext.JitteredViewProjectionMatrix);
  _Renderer.SetUniform("u_JitteredPrevVP", _RenderContext.PreviousJitteredViewProjectionMatrix);
  _Renderer.SetUniform("u_CurrentVP", _RenderContext.ViewProjectionMatrix);
  _Renderer.SetUniform("u_PreviousVP", _RenderContext.PreviousViewProjectionMatrix);
  _Renderer.SetUniform("u_IsShadowMapEnabled", _RenderContext.ShadowMap != CTexture::INVALID_TEXTURE);
  _Renderer.SetUniform("u_ShadowMap", TEXTURE_SHADOW_MAP_INDEX);
  _Renderer.SetUniform("u_IrradianceMap", TEXTURE_IRRADIANCE_MAP_INDEX);
  C2DTexture::Bind(TEXTURE_SHADOW_MAP_UNIT, _RenderContext.ShadowMap);
  CCubemap::Bind(TEXTURE_IRRADIANCE_MAP_UNIT, _RenderContext.IrradianceMap);
}

void COpaqueRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  for (const TRenderCommand *Command : _Commands)
  {
    _Renderer.SetUniform("u_Model", Command->ModelMatrix);
    _Renderer.SetUniform("u_MVP", _RenderContext.JitteredViewProjectionMatrix * Command->ModelMatrix);

    C2DTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, Command->Material.BaseColorTexture);
    C2DTexture::Bind(TEXTURE_NORMAL_UNIT, Command->Material.NormalTexture);
    C2DTexture::Bind(TEXTURE_EMISSIVE_UNIT, Command->Material.EmissiveTexture);
    C2DTexture::Bind(TEXTURE_METALLIC_ROUGHNESS_UNIT, Command->Material.MetallicRoughnessTexture);
    C2DTexture::Bind(TEXTURE_OCCLUSION_UNIT, Command->Material.OcclusionTexture);

    _Renderer.SetUniform("u_Material.BaseColorTexture", TEXTURE_BASIC_COLOR_INDEX);
    _Renderer.SetUniform("u_Material.NormalTexture", TEXTURE_NORMAL_INDEX);
    _Renderer.SetUniform("u_Material.EmissiveTexture", TEXTURE_EMISSIVE_INDEX);
    _Renderer.SetUniform("u_Material.MetallicRoughnessTexture", TEXTURE_METALLIC_ROUGHNESS_INDEX);
    _Renderer.SetUniform("u_Material.OcclusionTexture", TEXTURE_OCCLUSION_INDEX);
    _Renderer.SetUniform("u_Material.BaseColorTextureTexCoordIndex", Command->Material.BaseColorTextureTexCoordIndex);
    _Renderer.SetUniform("u_Material.NormalTextureTexCoordIndex", Command->Material.NormalTextureTexCoordIndex);
    _Renderer.SetUniform("u_Material.EmissiveTextureTexCoordIndex", Command->Material.EmissiveTextureTexCoordIndex);
    _Renderer.SetUniform("u_Material.MetallicRoughnessTextureTexCoordIndex", Command->Material.MetallicRoughnessTextureTexCoordIndex);
    _Renderer.SetUniform("u_Material.OcclusionTextureTexCoordIndex", Command->Material.OcclusionTextureTexCoordIndex);
    _Renderer.SetUniform("u_Material.BaseColorFactor", Command->Material.BaseColorFactor);
    _Renderer.SetUniform("u_Material.EmissiveFactor", Command->Material.EmissiveFactor);
    _Renderer.SetUniform("u_Material.MetallicFactor", Command->Material.MetallicFactor);
    _Renderer.SetUniform("u_Material.RoughnessFactor", Command->Material.RoughnessFactor);
    _Renderer.SetUniform("u_Material.OcclusionStrength", Command->Material.OcclusionStrength);
    _Renderer.SetUniform("u_Material.IsDoubleSided", Command->Material.IsDoubleSided);
    _Renderer.SetUniform("u_Material.AlphaMode", static_cast<int>(Command->Material.AlphaMode));
    _Renderer.SetUniform("u_Material.AlphaCutoff", Command->Material.AlphaCutoff);
    _Renderer.SetCullFace(Command->Material.IsDoubleSided ? ECullMode::None : ECullMode::Back);
    _Renderer.SetBlending(Command->Material.AlphaMode);

    Command->VAO->Bind();

    if (Command->IndexType != EIndexType::Absent)
      _Renderer.DrawElements(Command->PrimitiveMode, Command->IndicesCount, Command->IndexType);
    else
      _Renderer.DrawArrays(Command->PrimitiveMode, Command->IndicesCount);

    Command->VAO->Unbind();
  }
}

void COpaqueRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
}

bool COpaqueRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.RenderFlags.test(ERenderFlags_Opaque);
}

bool COpaqueRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}

bool COpaqueRenderPass::NeedsCommands() const
{
  return true;
}
