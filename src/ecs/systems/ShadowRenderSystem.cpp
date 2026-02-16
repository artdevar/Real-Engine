#include "pch.h"

#include "ShadowRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "utils/Resource.h"
#include "utils/Logger.h"
#include "graphics/Texture.h"
#include "interfaces/Renderer.h"
#include "graphics/RenderTypes.h"
#include "graphics/RenderTypes.h"
#include "graphics/Shader.h"
#include "graphics/ShaderTypes.h"
#include "engine/Config.h"

#define RENDER_DEBUG_QUAD 0 // Set to 1 to render the depth map on a quad for debugging purposes

namespace ecs
{
    CShadowRenderSystem::CShadowRenderSystem() : SHADOW_MAP_SIZE(CConfig::Instance().GetShadowMapSize())
    {
        // Empty
    }

    void CShadowRenderSystem::Init(CCoordinator *_Coordinator)
    {
        CSystem::Init(_Coordinator);

        CreateDepthMap();
        m_DepthShader = resource::LoadShader("Depth");

        m_DepthMapFBO.Bind();
        m_DepthMapFBO.AttachTexture(GL_DEPTH_ATTACHMENT, m_DepthMap->Get());
        m_DepthMapFBO.DisableColorBuffer();
        m_DepthMapFBO.Unbind();
    }

    void CShadowRenderSystem::RenderInternal(IRenderer &_Renderer)
    {
        const auto OldViewport = _Renderer.GetViewport();
        m_DepthMapFBO.Bind();
        PrepareRenderState(_Renderer, {SHADOW_MAP_SIZE, SHADOW_MAP_SIZE});

        _Renderer.SetUniform("u_LightSpaceMatrix", _Renderer.GetLightSpaceMatrix());

        for (ecs::TEntity Entity : m_Entities)
        {
            auto &TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
            auto &ModelComponent = m_Coordinator->GetComponent<TModelComponent>(Entity);

            for (TModelComponent::TPrimitiveData &Primitive : ModelComponent.Primitives)
            {
                if (Primitive.MaterialIndex >= 0 && Primitive.MaterialIndex < ModelComponent.Materials.size())
                {
                    TModelComponent::TMaterialData &Material = ModelComponent.Materials[Primitive.MaterialIndex];

                    CTextureBase::Bind(GL_TEXTURE_2D, TEXTURE_BASIC_COLOR_UNIT, Material.BaseColorTexture.Texture);
                    _Renderer.SetUniform("u_Material.BaseColorTexture", TEXTURE_BASIC_COLOR_INDEX);

                    _Renderer.SetUniform("u_Material.AlphaMode", static_cast<int>(Material.AlphaMode));
                    _Renderer.SetUniform("u_Material.AlphaCutoff", Material.AlphaCutoff);
                }

                glm::mat4 ModelMatrix = TransformComponent.WorldMatrix * Primitive.PrimitiveMatrix;
                _Renderer.SetUniform("u_Model", ModelMatrix);

                Primitive.VAO.Bind();
                std::visit(Overloaded{[&_Renderer, &Primitive](const TModelComponent::TIndicesData &Data)
                                      {
                                          _Renderer.DrawElements(Primitive.Mode, Data.Indices, Data.Type);
                                      },
                                      [&_Renderer, &Primitive](const TModelComponent::TVerticesData &Data)
                                      {
                                          _Renderer.DrawArrays(Primitive.Mode, Data.Vertices);
                                      }},
                           Primitive.DrawData);
                Primitive.VAO.Unbind();
            }
        }

        _Renderer.SetShadowMap(m_DepthMap); // TODO: We don't need to set the shadow map every tick
        m_DepthMapFBO.Unbind();
        RestoreRenderState(_Renderer, OldViewport);

#if RENDER_DEBUG_QUAD
        RenderDebugQuad(_Renderer);
#endif
    }

    void CShadowRenderSystem::PrepareRenderState(IRenderer &_Renderer, TVector2i _NewViewport)
    {
        _Renderer.Clear(EClearFlags::Depth);
        _Renderer.SetShader(m_DepthShader.lock());
        _Renderer.SetViewport(_NewViewport);
        _Renderer.SetCullFace(ECullMode::Front);
    }

    void CShadowRenderSystem::RestoreRenderState(IRenderer &_Renderer, TVector2i _OldViewport)
    {
        _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Depth | EClearFlags::Color));
        _Renderer.SetViewport(_OldViewport);
    }

    bool CShadowRenderSystem::ShouldBeRendered() const
    {
        return CConfig::Instance().GetShadowsEnabled() &&
               !m_Entities.Empty() &&
               !m_DepthShader.expired();
    }

    void CShadowRenderSystem::CreateDepthMap()
    {
        TTextureParams DepthMapParams;
        DepthMapParams.Width = SHADOW_MAP_SIZE;
        DepthMapParams.Height = SHADOW_MAP_SIZE;
        DepthMapParams.InternalFormat = GL_DEPTH_COMPONENT16;
        DepthMapParams.Format = GL_DEPTH_COMPONENT;
        DepthMapParams.Type = GL_FLOAT;
        DepthMapParams.BorderColors.emplace({1.0f, 1.0f, 1.0f, 1.0f});
        DepthMapParams.WrapS = ETextureWrap::ClampToBorder;
        DepthMapParams.WrapT = ETextureWrap::ClampToBorder;
        DepthMapParams.MinFilter = ETextureFilter::Nearest;
        DepthMapParams.MagFilter = ETextureFilter::Nearest;
        m_DepthMap = resource::CreateTexture("ShadowDepthMap", DepthMapParams);
    }

    void CShadowRenderSystem::RenderDebugQuad(IRenderer &_Renderer)
    {
        static CVertexArray VAO = [this]()
        {
            constexpr float QuadVertices[] = {
                // positions   // texcoords
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

            CVertexArray VAO;
            CVertexBuffer VBO(GL_STATIC_DRAW);
            VAO.Bind();
            VBO.Bind();
            VBO.Assign(QuadVertices, sizeof(QuadVertices));
            VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 5 * sizeof(float), (void *)0);
            VAO.EnableAttrib(ATTRIB_LOC_TEXCOORDS_0, 2, GL_FLOAT, false, 5 * sizeof(float), (void *)(3 * sizeof(float)));
            VAO.Unbind();

            return VAO;
        }();

        static std::shared_ptr<CShader> DebugShader = resource::LoadShader("DebugDepth");
        _Renderer.SetShader(DebugShader);

        m_DepthMap->Bind(GL_TEXTURE0);
        _Renderer.SetUniform("u_DepthMap", 0);
        _Renderer.SetUniform("u_NearPlane", CConfig::Instance().GetLightSpaceMatrixZNear());
        _Renderer.SetUniform("u_FarPlane", CConfig::Instance().GetLightSpaceMatrixZFar());

        VAO.Bind();
        _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
        VAO.Unbind();
    }
}