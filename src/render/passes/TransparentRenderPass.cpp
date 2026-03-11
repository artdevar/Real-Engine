#include "TransparentRenderPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "render/RenderTarget.h"
#include "assets/Texture.h"
#include "engine/Camera.h"
#include "utils/Resource.h"

CTransparentRenderPass::CTransparentRenderPass() :
    m_Shader(resource::LoadShader("PBR"))
{
}

void CTransparentRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthFunc(GL_LESS);
  _Renderer.SetDepthMask(false);
  _Renderer.SetCullFace(ECullMode::Back);
  _Renderer.SetBlending(EAlphaMode::Blend);
  _Renderer.SetViewport(_RenderContext.SceneRenderTarget.Size);
  _Renderer.SetShader(m_Shader);
  _Renderer.SetUniform("u_ViewPos", _RenderContext.CameraPosition);
  _Renderer.SetUniform("u_LightSpaceMatrix", _RenderContext.LightSpaceMatrix);
  _Renderer.SetUniform("u_ShadowMap", TEXTURE_SHADOW_MAP_INDEX);
  _Renderer.SetUniform("u_IrradianceMap", TEXTURE_IRRADIANCE_MAP_INDEX);
  CTexture::Bind(TEXTURE_SHADOW_MAP_UNIT, _RenderContext.ShadowMap);
  CCubemap::Bind(TEXTURE_IRRADIANCE_MAP_UNIT, _RenderContext.IrradianceMap);
}

void CTransparentRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  for (const TRenderCommand *Command : _Commands)
  {
    _Renderer.SetUniform("u_Model", Command->ModelMatrix);
    _Renderer.SetUniform("u_MVP", _RenderContext.ViewProjectionMatrix * Command->ModelMatrix);

    CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, Command->Material.BaseColorTexture);
    CTexture::Bind(TEXTURE_NORMAL_UNIT, Command->Material.NormalTexture);
    CTexture::Bind(TEXTURE_EMISSIVE_UNIT, Command->Material.EmissiveTexture);
    CTexture::Bind(TEXTURE_METALLIC_ROUGHNESS_UNIT, Command->Material.MetallicRoughnessTexture);
    CTexture::Bind(TEXTURE_OCCLUSION_UNIT, Command->Material.OcclusionTexture);

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

    Command->VAO->Bind();

    if (Command->IndexType != EIndexType::Absent)
      _Renderer.DrawElements(Command->PrimitiveMode, Command->IndicesCount, Command->IndexType);
    else
      _Renderer.DrawArrays(Command->PrimitiveMode, Command->IndicesCount);

    Command->VAO->Unbind();
  }
}

void CTransparentRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthMask(true); // Required for clearing the depth buffer
}

bool CTransparentRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.RenderFlags.test(ERenderFlags_Transparent);
}

bool CTransparentRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}

bool CTransparentRenderPass::NeedsCommands() const
{
  return true;
}