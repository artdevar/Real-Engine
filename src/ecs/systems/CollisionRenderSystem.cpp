#include "pch.h"

#include "CollisionRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "render/RenderCommand.h"
#include "render/RenderQueue.h"
#include "render/Buffer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ecs
{

void CCollisionRenderSystem::Collect(CRenderQueue &_Queue)
{
  for (ecs::TEntity Entity : m_SelectedEntities)
  {
    if (!m_Entities.Contains(Entity))
      continue;

    TTransformComponent &TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
    TCollisionComponent &CollisionComponent = m_Coordinator->GetComponent<TCollisionComponent>(Entity);

    if (!CollisionComponent.VAO)
      return;

    const TAABB    &Box    = CollisionComponent.BoundingBox;
    const glm::mat4 Model  = TransformComponent.WorldMatrix;
    const glm::vec3 Center = Box.Center();
    const glm::vec3 Size   = Box.Size();

    glm::mat4 BoxMatrix = glm::translate(Model, Center);
    BoxMatrix           = glm::scale(BoxMatrix, Size);

    TRenderFlags RenderFlags;
    RenderFlags.set(ERenderFlags_Wireframe);

    TRenderCommand Command{
        .VAO           = *CollisionComponent.VAO,
        .ModelMatrix   = BoxMatrix,
        .IndicesCount  = 24,
        .IndexType     = EIndexType::Absent,
        .PrimitiveMode = EPrimitiveMode::Line,
        .RenderFlags   = std::move(RenderFlags),
    };

    _Queue.Push(std::move(Command));
  }
}

void CCollisionRenderSystem::OnEntitySelected(ecs::TEntity _Entity)
{
  m_SelectedEntities.PushUnique(_Entity);
}

void CCollisionRenderSystem::OnEntityDeselected(ecs::TEntity _Entity)
{
  m_SelectedEntities.SafeErase(_Entity);
}

} // namespace ecs
