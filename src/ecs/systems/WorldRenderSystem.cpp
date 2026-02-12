#include "pch.h"

#include "WorldRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "graphics/Shader.h"
#include "graphics/ShaderTypes.h"
#include "graphics/Renderer.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"
#include "engine/Camera.h"
#include "utils/Common.h"
#include "utils/Resource.h"
#include "tiny_gltf.h"

#include "ecs/systems/ShadowRenderSystem.h"
namespace ecs
{
    static void ActivateTexture(
        CRenderer &_Renderer,
        const std::shared_ptr<CTextureBase> &_Texture,
        GLint _TextureIndex,
        std::string_view _UniformName)
    {
        if (_Texture)
        {
            _Texture->Bind(GL_TEXTURE0 + _TextureIndex);
            _Renderer.SetUniform(_UniformName, _TextureIndex);
        }
    }

    CWorldRenderSystem::CWorldRenderSystem() = default;

    void CWorldRenderSystem::Init(CCoordinator *_Coordinator)
    {
        CSystem::Init(_Coordinator);

        // m_ModelShader = resource::LoadShader("pbr");
        m_ModelShader = resource::LoadShader("basic");
    }

    void CWorldRenderSystem::RenderInternal(CRenderer &_Renderer)
    {
        const std::shared_ptr<CCamera> &Camera = _Renderer.GetCamera();
        const glm::mat4 ViewProjection = Camera->GetProjection() * Camera->GetView();

        _Renderer.SetShader(m_ModelShader.lock());
        _Renderer.SetUniform("u_ViewPos", Camera->GetPosition());
        _Renderer.SetUniform("u_LightSpaceMatrix", _Renderer.GetLightSpaceMatrix());

        { // TODO: Remove this dependency on ShadowRenderSystem

            std::shared_ptr<CTextureBase> ShadowMapTexture;
            if (auto ShadowSystem = m_Coordinator->GetSystem<ecs::CShadowRenderSystem>())
                ShadowMapTexture = ShadowSystem->m_DepthMap;

            ActivateTexture(_Renderer, ShadowMapTexture, TEXTURE_SHADOW_MAP_INDEX, "u_ShadowMap");
        }

        for (ecs::TEntity Entity : m_Entities)
        {
            auto &TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
            auto &ModelComponent = m_Coordinator->GetComponent<TModelComponent>(Entity);

            _Renderer.SetUniform("u_Model", TransformComponent.Transform);
            _Renderer.SetUniform("u_MVP", ViewProjection * TransformComponent.Transform);

            for (TModelComponent::TPrimitiveData &Primitive : ModelComponent.Primitives)
            {
                if (Primitive.MaterialIndex >= 0 && Primitive.MaterialIndex < ModelComponent.Materials.size())
                {
                    TModelComponent::TMaterialData &Material = ModelComponent.Materials[Primitive.MaterialIndex];

                    ActivateTexture(_Renderer, Material.BaseColorTexture, TEXTURE_BASIC_COLOR_INDEX, "u_Material.BaseColorTexture");
                    ActivateTexture(_Renderer, Material.NormalTexture, TEXTURE_NORMAL_INDEX, "u_Material.NormalTexture");
                    ActivateTexture(_Renderer, Material.EmissiveTexture, TEXTURE_EMISSIVE_INDEX, "u_Material.EmissiveTexture");
                    ActivateTexture(_Renderer, Material.MetallicRoughnessTexture, TEXTURE_METALLIC_ROUGHNESS_INDEX, "u_Material.RoughnessTexture");

                    _Renderer.SetUniform("u_Material.BaseColorFactor", Material.BaseColorFactor);
                    _Renderer.SetUniform("u_Material.EmissiveFactor", Material.EmissiveFactor);
                    _Renderer.SetUniform("u_Material.MetallicFactor", Material.MetallicFactor);
                    _Renderer.SetUniform("u_Material.RoughnessFactor", Material.RoughnessFactor);
                    _Renderer.SetUniform("u_Material.IsDoubleSided", Material.IsDoubleSided);
                    _Renderer.SetUniform("u_Material.AlphaMode", static_cast<int>(Material.AlphaMode));
                    _Renderer.SetUniform("u_Material.AlphaCutoff", Material.AlphaCutoff);
                    _Renderer.SetCullFace(Material.IsDoubleSided ? GL_NONE : GL_BACK);
                    _Renderer.SetBlending(Material.AlphaMode);
                }

                Primitive.VAO.Bind();
                _Renderer.DrawElements(GL_TRIANGLES, Primitive.Indices, GL_UNSIGNED_INT, (int8_t *)0 + Primitive.Offset);
                Primitive.VAO.Unbind();
            }
        }
        _Renderer.SetBlending(EAlphaMode::Opaque);
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
}