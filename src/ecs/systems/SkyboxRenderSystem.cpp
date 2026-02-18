#include "pch.h"

#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "ecs/systems/SkyboxRenderSystem.h"
#include "utils/Resource.h"
#include "render/RenderCommand.h"
#include "render/RenderQueue.h"

namespace ecs
{

void CSkyboxRenderSystem::Collect(CRenderQueue &_Queue)
{
  if (m_Entities.Empty())
    return;

  assert(m_Entities.Size() == 1 && "It isn't supposed to be more than 1 skybox");

  ecs::TEntity Entity          = m_Entities[0];
  auto        &SkyboxComponent = m_Coordinator->GetComponent<TSkyboxComponent>(Entity);

  TRenderCommand Command{
      .Material      = TMaterialD{.SkyboxTexture = SkyboxComponent.SkyboxTexture},
      .VAO           = SkyboxComponent.VAO,
      .ModelMatrix   = glm::mat4(1.0f),
      .IndicesCount  = SkyboxComponent.VerticesCount,
      .IndexType     = EIndexType::None,
      .PrimitiveMode = EPrimitiveMode::Triangles,
  };

  _Queue.Push(std::move(Command));
}

} // namespace ecs
