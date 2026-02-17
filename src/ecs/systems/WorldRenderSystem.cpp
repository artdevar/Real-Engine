#include "pch.h"

#include "WorldRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "engine/Camera.h"
#include "engine/Config.h"
#include "graphics/Model.h"
#include "graphics/RenderTypes.h"
#include "graphics/Shader.h"
#include "graphics/ShaderTypes.h"
#include "graphics/Texture.h"
#include "interfaces/Renderer.h"
#include "utils/Common.h"
#include "utils/Logger.h"
#include "utils/Resource.h"

namespace ecs
{
CWorldRenderSystem::CWorldRenderSystem() = default;

void CWorldRenderSystem::Init(CCoordinator *_Coordinator)
{
  CSystem::Init(_Coordinator);

  // m_ModelShader = resource::LoadShader("PBR");
  m_ModelShader = resource::LoadShader("Basic");
}

void CWorldRenderSystem::RenderInternal(IRenderer &_Renderer)
{
  PrepareRenderState(_Renderer);

  const std::shared_ptr<CCamera> &Camera         = _Renderer.GetCamera();
  const glm::mat4                 ViewProjection = Camera->GetProjection() * Camera->GetView();

  _Renderer.SetUniform("u_ViewPos", Camera->GetPosition());
  _Renderer.SetUniform("u_LightSpaceMatrix", _Renderer.GetLightSpaceMatrix());

  const auto &ShadowMapTexture = _Renderer.GetShadowMap();
  if (ShadowMapTexture)
  {
    ShadowMapTexture->Bind(TEXTURE_SHADOW_MAP_UNIT);
    _Renderer.SetUniform("u_ShadowMap", TEXTURE_SHADOW_MAP_INDEX);
  }

  for (ecs::TEntity Entity : m_Entities)
  {
    auto &ModelComponent = m_Coordinator->GetComponent<TModelComponent>(Entity);
    for (TModelComponent::TPrimitiveData &Primitive : ModelComponent.Primitives)
    {
      if (Primitive.MaterialIndex >= 0 && Primitive.MaterialIndex < ModelComponent.Materials.size())
      {
        TModelComponent::TMaterialData &Material = ModelComponent.Materials[Primitive.MaterialIndex];

        CTexture::Bind(CTexture::TARGET, TEXTURE_BASIC_COLOR_UNIT, Material.BaseColorTexture.Texture);
        CTexture::Bind(CTexture::TARGET, TEXTURE_NORMAL_UNIT, Material.NormalTexture.Texture);
        CTexture::Bind(CTexture::TARGET, TEXTURE_EMISSIVE_UNIT, Material.EmissiveTexture.Texture);
        CTexture::Bind(CTexture::TARGET, TEXTURE_METALLIC_ROUGHNESS_UNIT, Material.MetallicRoughnessTexture.Texture);

        _Renderer.SetUniform("u_Material.BaseColorTexture", TEXTURE_BASIC_COLOR_INDEX);
        _Renderer.SetUniform("u_Material.NormalTexture", TEXTURE_NORMAL_INDEX);
        _Renderer.SetUniform("u_Material.EmissiveTexture", TEXTURE_EMISSIVE_INDEX);
        _Renderer.SetUniform("u_Material.MetallicRoughnessTexture", TEXTURE_METALLIC_ROUGHNESS_INDEX);
        _Renderer.SetUniform("u_Material.BaseColorTextureTexCoordIndex", Material.BaseColorTexture.TexCoordIndex);
        _Renderer.SetUniform("u_Material.NormalTextureTexCoordIndex", Material.NormalTexture.TexCoordIndex);
        _Renderer.SetUniform("u_Material.EmissiveTextureTexCoordIndex", Material.EmissiveTexture.TexCoordIndex);
        _Renderer.SetUniform("u_Material.MetallicRoughnessTextureTexCoordIndex", Material.MetallicRoughnessTexture.TexCoordIndex);
        _Renderer.SetUniform("u_Material.BaseColorFactor", Material.BaseColorFactor);
        _Renderer.SetUniform("u_Material.EmissiveFactor", Material.EmissiveFactor);
        _Renderer.SetUniform("u_Material.MetallicFactor", Material.MetallicFactor);
        _Renderer.SetUniform("u_Material.RoughnessFactor", Material.RoughnessFactor);
        _Renderer.SetUniform("u_Material.IsDoubleSided", Material.IsDoubleSided);
        _Renderer.SetUniform("u_Material.AlphaMode", static_cast<int>(Material.AlphaMode));
        _Renderer.SetUniform("u_Material.AlphaCutoff", Material.AlphaCutoff);

        _Renderer.SetCullFace(Material.IsDoubleSided ? ECullMode::None : ECullMode::Back);
        _Renderer.SetBlending(Material.AlphaMode);
      }
      else
      {
        CLogger::Log(ELogType::Warning, "Primitive with invalid material index: " + std::to_string(Primitive.MaterialIndex));
      }

      auto     &TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
      glm::mat4 ModelMatrix        = TransformComponent.WorldMatrix * Primitive.PrimitiveMatrix;
      _Renderer.SetUniform("u_MVP", ViewProjection * ModelMatrix);
      _Renderer.SetUniform("u_Model", ModelMatrix);

      Primitive.VAO.Bind();
      std::visit(Overloaded{[&_Renderer, &Primitive](const TModelComponent::TIndicesData &Data) {
                              _Renderer.DrawElements(Primitive.Mode, Data.Indices, Data.Type);
                            },
                            [&_Renderer, &Primitive](const TModelComponent::TVerticesData &Data) {
                              _Renderer.DrawArrays(Primitive.Mode, Data.Vertices);
                            }},
                 Primitive.DrawData);
      Primitive.VAO.Unbind();
    }
  }
}

void CWorldRenderSystem::PrepareRenderState(IRenderer &_Renderer)
{
  _Renderer.SetShader(m_ModelShader.lock());
  _Renderer.SetCullFace(ECullMode::Back);
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthMask(true);
  _Renderer.SetDepthFunc(GL_LESS);
}

void CWorldRenderSystem::SetVisibility(ecs::TEntity _Entity, bool _IsVisible)
{
  if (_IsVisible)
  {
    assert(m_HiddenEntities.Contains(_Entity));
    assert(!m_Entities.Contains(_Entity));

    m_Entities.Push(_Entity);
    m_HiddenEntities.Erase(_Entity);
  }
  else
  {
    assert(!m_HiddenEntities.Contains(_Entity));
    assert(m_Entities.Contains(_Entity));

    m_Entities.Erase(_Entity);
    m_HiddenEntities.Push(_Entity);
  }
}

bool CWorldRenderSystem::ShouldBeRendered() const
{
  return !m_Entities.Empty();
}

void CWorldRenderSystem::OnEntityAdded(ecs::TEntity _Entity)
{
  CSystem::OnEntityAdded(_Entity);
}

void CWorldRenderSystem::OnEntityDeleted(ecs::TEntity _Entity)
{
  CSystem::OnEntityDeleted(_Entity);

  m_HiddenEntities.SafeErase(_Entity);
}
} // namespace ecs