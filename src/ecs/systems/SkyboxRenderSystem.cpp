#include "pch.h"

#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "ecs/systems/SkyboxRenderSystem.h"
#include "utils/Resource.h"
#include "render/RenderCommand.h"
#include "render/RenderQueue.h"
#include "assets/Texture.h"

namespace ecs
{

void CSkyboxRenderSystem::Collect(CRenderQueue &_Queue)
{
  if (m_Entities.Empty())
    return;

  ecs::TEntity Entity          = m_Entities[0];
  auto        &SkyboxComponent = m_Coordinator->GetComponent<TSkyboxComponent>(Entity);

  TRenderFlags RenderFlags;
  RenderFlags.set(ERenderFlags_Skybox);

  TRenderCommand Command{
      .Material    = TMaterialD{.SkyboxTexture = SkyboxComponent.SkyboxTexture ? SkyboxComponent.SkyboxTexture->ID() : CCubemap::INVALID_VALUE},
      .ModelMatrix = glm::mat4(1.0f),
      .RenderFlags = std::move(RenderFlags),
  };

  _Queue.Push(std::move(Command));
}

void CSkyboxRenderSystem::OnEntityAdded(ecs::TEntity _Entity)
{
  assert(m_Entities.Size() == 1 && "It isn't supposed to be more than 1 skybox");
}

} // namespace ecs
