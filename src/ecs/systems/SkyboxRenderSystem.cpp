#include "pch.h"

#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "ecs/systems/SkyboxRenderSystem.h"
#include "engine/Camera.h"
#include "graphics/Buffer.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "interfaces/Renderer.h"
#include "utils/Resource.h"

namespace ecs
{
void CSkyboxRenderSystem::Init(CCoordinator *_Coordinator)
{
  CSystem::Init(_Coordinator);

  m_SkyboxShader = resource::LoadShader("Skybox");
}

void CSkyboxRenderSystem::RenderInternal(IRenderer &_Renderer)
{
  PrepareRenderState(_Renderer);

  const std::shared_ptr<CCamera> &Camera = _Renderer.GetCamera();

  _Renderer.SetUniform("u_View", glm::mat4(glm::mat3(_Renderer.GetCamera()->GetView())));
  _Renderer.SetUniform("u_Projection", Camera->GetProjection());

  assert(m_Entities.Size() == 1); // it isn't supposed to be more than 1 skybox

  ecs::TEntity Entity          = m_Entities[0];
  auto        &SkyboxComponent = m_Coordinator->GetComponent<TSkyboxComponent>(Entity);

  CCubemap::Bind(CCubemap::TARGET, TEXTURE_SKYBOX_UNIT, SkyboxComponent.SkyboxTexture);
  _Renderer.SetUniform("u_Cubemap", TEXTURE_SKYBOX_INDEX);

  SkyboxComponent.VAO.Bind();
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, SkyboxComponent.VerticesCount);
  SkyboxComponent.VAO.Unbind();
}

bool CSkyboxRenderSystem::ShouldBeRendered() const
{
  return !m_Entities.Empty();
}

void CSkyboxRenderSystem::PrepareRenderState(IRenderer &_Renderer)
{
  _Renderer.SetShader(m_SkyboxShader.lock());
  _Renderer.SetCullFace(ECullMode::None);
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthMask(true);
  _Renderer.SetDepthFunc(GL_LEQUAL);
}

} // namespace ecs
