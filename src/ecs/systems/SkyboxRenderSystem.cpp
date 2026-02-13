#include "pch.h"

#include "ecs/systems/SkyboxRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Buffer.h"
#include "graphics/RenderTypes.h"
#include "engine/Engine.h"
#include "engine/ResourceManager.h"
#include "engine/Camera.h"
#include "utils/Common.h"

namespace ecs
{
    static constexpr inline float SKYBOX_VERTICES[] = {
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    CSkyboxRenderSystem::CSkyboxRenderSystem() : m_VBO(GL_STATIC_DRAW)
    {
    }

    void CSkyboxRenderSystem::Init(CCoordinator *_Coordinator)
    {
        CSystem::Init(_Coordinator);

        m_SkyboxShader = CEngine::Instance().GetResourceManager()->LoadShader("skybox");

        m_VAO.Bind();
        m_VBO.Bind();
        m_VBO.Assign(SKYBOX_VERTICES, sizeof(SKYBOX_VERTICES));
        m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, 3 * sizeof(float), (GLvoid *)0);
        m_VAO.Unbind();
    }

    void CSkyboxRenderSystem::RenderInternal(IRenderer &_Renderer)
    {
        std::shared_ptr<CShader> Shader = m_SkyboxShader.lock();

        _Renderer.SetShader(Shader);
        _Renderer.SetUniform("u_View", glm::mat4(glm::mat3(_Renderer.GetCamera()->GetView())));

        glDepthFunc(GL_LEQUAL);

        assert(m_Entities.Size() == 1); // it isn't supposed to be more than 1 skybox

        m_VAO.Bind();

        ecs::TEntity Entity = m_Entities[0];
        auto &SkyboxComponent = m_Coordinator->GetComponent<TSkyboxComponent>(Entity);

        SkyboxComponent.SkyboxTexture->Bind(TEXTURE_SKYBOX_UNIT);
        _Renderer.SetUniform("u_Cubemap", TEXTURE_SKYBOX_INDEX);
        _Renderer.DrawArrays(EPrimitiveMode::Triangles, ARRAY_SIZE(SKYBOX_VERTICES) / 3);

        SkyboxComponent.SkyboxTexture->Unbind();
        m_VAO.Unbind();

        glDepthFunc(GL_LESS);
    }

    bool CSkyboxRenderSystem::ShouldBeRendered() const
    {
        return !m_Entities.Empty();
    }

}
