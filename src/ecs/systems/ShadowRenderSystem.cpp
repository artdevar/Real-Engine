#include "pch.h"

#include "ShadowRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "utils/Resource.h"
#include "graphics/Texture.h"
#include "graphics/Renderer.h"
#include "graphics/Shader.h"

namespace ecs
{
    CShadowRenderSystem::CShadowRenderSystem() = default;

    void CShadowRenderSystem::Init(CCoordinator *_Coordinator)
    {
        CSystem::Init(_Coordinator);

        TTextureParams DepthMapParams;
        DepthMapParams.Width = SHADOW_MAP_SIZE;
        DepthMapParams.Height = SHADOW_MAP_SIZE;
        DepthMapParams.WrapS = ETextureWrap::ClampToEdge;
        DepthMapParams.WrapT = ETextureWrap::ClampToEdge;
        m_DepthMap = resource::CreateTexture("ShadowDepthMap", DepthMapParams);
        m_DepthShader = resource::LoadShader("../shaders/depth");

        m_DepthMapFBO.Bind();
        m_DepthMapFBO.AttachTexture(m_DepthMap->Get());
        m_DepthMapFBO.DisableColorBuffer();
        m_DepthMapFBO.Unbind();
    }

    void CShadowRenderSystem::RenderInternal(CRenderer &_Renderer)
    {
        const auto OldViewport = _Renderer.GetViewport();

        m_DepthMapFBO.Bind();
        _Renderer.SetViewport(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        _Renderer.Clear(GL_DEPTH_BUFFER_BIT);
        _Renderer.SetShader(m_DepthShader);
        _Renderer.SetUniform("u_LightSpaceMatrix", _Renderer.GetLightSpaceMatrix());

        for (ecs::TEntity Entity : m_Entities)
        {
            auto &TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
            auto &ModelComponent = m_Coordinator->GetComponent<TModelComponent>(Entity);

            _Renderer.SetUniform("u_Model", TransformComponent.Transform);

            for (TModelComponent::TPrimitiveData &Primitive : ModelComponent.Primitives)
            {
                Primitive.VAO.Bind();
                _Renderer.DrawElements(GL_TRIANGLES, Primitive.Indices, GL_UNSIGNED_INT, (int8_t *)0 + Primitive.Offset);
                Primitive.VAO.Unbind();
            }
        }

        m_DepthMapFBO.Unbind();

        _Renderer.SetViewport(OldViewport.x, OldViewport.y);
        _Renderer.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    bool CShadowRenderSystem::ShouldBeRendered() const
    {
        return !m_Entities.Empty();
    }
}