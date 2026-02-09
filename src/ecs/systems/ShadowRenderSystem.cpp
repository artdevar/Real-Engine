#include "pch.h"

#include "ShadowRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "utils/Resource.h"
#include "graphics/Texture.h"
#include "graphics/Renderer.h"
#include "graphics/Shader.h"

#include "graphics/ShaderTypes.h"
#include "engine/Config.h"

namespace ecs
{
    CShadowRenderSystem::CShadowRenderSystem() = default;

    void CShadowRenderSystem::Init(CCoordinator *_Coordinator)
    {
        CSystem::Init(_Coordinator);

        TTextureParams DepthMapParams;
        DepthMapParams.Width = SHADOW_MAP_SIZE;
        DepthMapParams.Height = SHADOW_MAP_SIZE;
        DepthMapParams.BorderColors.emplace({1.0f, 1.0f, 1.0f, 1.0f});
        DepthMapParams.WrapS = ETextureWrap::ClampToBorder;
        DepthMapParams.WrapT = ETextureWrap::ClampToBorder;
        DepthMapParams.MinFilter = ETextureFilter::Nearest;
        DepthMapParams.MagFilter = ETextureFilter::Nearest;
        m_DepthMap = resource::CreateTexture("ShadowDepthMap", DepthMapParams);
        m_DepthShader = resource::LoadShader("depth");

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
        _Renderer.SetCullFace(GL_FRONT);
        _Renderer.SetUniform("u_LightSpaceMatrix", _Renderer.GetLightSpaceMatrix());

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

                    _Renderer.SetUniform("u_Material.AlphaMode", static_cast<int>(Material.AlphaMode));
                    _Renderer.SetUniform("u_Material.AlphaCutoff", Material.AlphaCutoff);
                    _Renderer.SetAlphaBlending(Material.AlphaMode == EAlphaMode::Blend);
                }
                Primitive.VAO.Bind();
                _Renderer.DrawElements(GL_TRIANGLES, Primitive.Indices, GL_UNSIGNED_INT, (int8_t *)0 + Primitive.Offset);
                Primitive.VAO.Unbind();
            }
        }

        m_DepthMapFBO.Unbind();

        _Renderer.SetViewport(OldViewport.x, OldViewport.y);
        _Renderer.SetCullFace(GL_BACK);
        _Renderer.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // RenderDebugQuad(_Renderer);
    }

    bool CShadowRenderSystem::ShouldBeRendered() const
    {
        return !m_Entities.Empty();
    }

    void CShadowRenderSystem::RenderDebugQuad(CRenderer &_Renderer)
    {
        static CVertexArray VAO = [this]()
        {
            constexpr float QuadVertices[] = {
                -1.0f,
                1.0f,
                0.0f,
                0.0f,
                1.0f,
                -1.0f,
                -1.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f,
                1.0f,
                0.0f,
                1.0f,
                1.0f,
                1.0f,
                -1.0f,
                0.0f,
                1.0f,
                0.0f,
            };

            CVertexArray VAO;
            CVertexBuffer VBO(GL_STATIC_DRAW);
            VAO.Bind();
            VBO.Bind();
            VBO.Assign(QuadVertices, sizeof(QuadVertices));
            VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, 5 * sizeof(float), (void *)0);
            VAO.EnableAttrib(ATTRIB_LOC_TEXCOORDS, 2, GL_FLOAT, 5 * sizeof(float), (void *)(3 * sizeof(float)));
            VAO.Unbind();

            return VAO;
        }();

        static std::shared_ptr<CShader> DebugShader = resource::LoadShader("debug_depth");
        _Renderer.SetShader(DebugShader);

        m_DepthMap->Bind(GL_TEXTURE0);
        _Renderer.SetUniform("u_DepthMap", 0);
        _Renderer.SetUniform("u_NearPlane", CConfig::Instance().GetLightSpaceMatrixZNear());
        _Renderer.SetUniform("u_FarPlane", CConfig::Instance().GetLightSpaceMatrixZFar());

        VAO.Bind();
        _Renderer.DrawArrays(GL_TRIANGLE_STRIP, 4);
        VAO.Unbind();
    }
}