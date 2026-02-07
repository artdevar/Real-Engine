#include "pch.h"

#include "ModelRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "graphics/Shader.h"
#include "graphics/ShaderTypes.h"
#include "graphics/Renderer.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"
#include "engine/Engine.h"
#include "engine/ResourceManager.h"
#include "engine/Camera.h"
#include "utils/Common.h"
#include "tiny_gltf.h"

namespace ecs
{

    CModelRenderSystem::CModelRenderSystem()
    {
    }

    void CModelRenderSystem::Init(CCoordinator *_Coordinator)
    {
        CSystem::Init(_Coordinator);

        m_ModelShader = CEngine::Instance().GetResourceManager()->LoadShader("../shaders/pbr");
    }

    void CModelRenderSystem::Render(CRenderer &_Renderer)
    {
        if (m_Entities.Empty())
            return;

        std::shared_ptr<CShader> Shader = m_ModelShader.lock();
        _Renderer.SetShader(Shader);

        for (ecs::TEntity Entity : m_Entities)
        {
            auto &TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
            auto &ModelComponent = m_Coordinator->GetComponent<TModelComponent>(Entity);

            _Renderer.SetUniform("u_Model", TransformComponent.Transform);

            for (TModelComponent::TPrimitiveData &Primitive : ModelComponent.Primitives)
            {
                if (Primitive.MaterialIndex >= 0 && Primitive.MaterialIndex < ModelComponent.Materials.size())
                {
                    TModelComponent::TMaterialData &Material = ModelComponent.Materials[Primitive.MaterialIndex];

                    int TextureUnit = 0;

                    if (Material.BaseColorTexture)
                    {
                        Material.BaseColorTexture->Bind(GL_TEXTURE0 + TextureUnit);
                        _Renderer.SetUniform("u_Material.BaseColorTexture", TextureUnit);
                        ++TextureUnit;
                    }

                    if (Material.MetallicRoughnessTexture)
                    {
                        Material.MetallicRoughnessTexture->Bind(GL_TEXTURE0 + TextureUnit);
                        _Renderer.SetUniform("u_Material.RoughnessTexture", TextureUnit);
                        ++TextureUnit;
                    }

                    if (Material.NormalTexture)
                    {
                        Material.NormalTexture->Bind(GL_TEXTURE0 + TextureUnit);
                        _Renderer.SetUniform("u_Material.NormalTexture", TextureUnit);
                        ++TextureUnit;
                    }

                    _Renderer.SetUniform("u_Material.BaseColorFactor", Material.BaseColorFactor);
                    _Renderer.SetUniform("u_Material.MetallicFactor", Material.MetallicFactor);
                    _Renderer.SetUniform("u_Material.RoughnessFactor", Material.RoughnessFactor);
                }

                Primitive.VAO.Bind();
                glDrawElements(GL_TRIANGLES, Primitive.Indices, GL_UNSIGNED_INT, (int8_t *)0 + Primitive.Offset);
                Primitive.VAO.Unbind();
            }
        }
    }

    void CModelRenderSystem::SetVisibility(ecs::TEntity _Entity, bool _IsVisible)
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

    void CModelRenderSystem::OnEntityAdded(ecs::TEntity _Entity)
    {
        CSystem::OnEntityAdded(_Entity);
    }

    void CModelRenderSystem::OnEntityDeleted(ecs::TEntity _Entity)
    {
        CSystem::OnEntityDeleted(_Entity);

        m_HiddenEntities.SafeErase(_Entity);
    }

}