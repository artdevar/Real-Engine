#include "pch.h"

#include "ModelRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "render/RenderCommand.h"
#include "render/RenderQueue.h"
#include "assets/Texture.h"

namespace ecs
{

void CModelRenderSystem::Collect(CRenderQueue &_Queue)
{
  constexpr auto GetTextureID = [](const TModelComponent::TTexture &_Texture) -> uint32_t {
    return _Texture.Texture ? _Texture.Texture->ID() : 0;
  };

  for (ecs::TEntity Entity : m_Entities)
  {
    TModelComponent &ModelComponent = m_Coordinator->GetComponent<TModelComponent>(Entity);
    for (TModelComponent::TPrimitiveData &Primitive : ModelComponent.Primitives)
    {
      TTransformComponent            &TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
      TModelComponent::TMaterialData &Material           = ModelComponent.Materials[Primitive.MaterialIndex];

      TRenderCommand Command{
          .Material =
              TMaterialD{
                  .BaseColorTexture                      = GetTextureID(Material.BaseColorTexture),
                  .BaseColorTextureTexCoordIndex         = Material.BaseColorTexture.TexCoordIndex,
                  .NormalTexture                         = GetTextureID(Material.NormalTexture),
                  .NormalTextureTexCoordIndex            = Material.NormalTexture.TexCoordIndex,
                  .MetallicRoughnessTexture              = GetTextureID(Material.MetallicRoughnessTexture),
                  .MetallicRoughnessTextureTexCoordIndex = Material.MetallicRoughnessTexture.TexCoordIndex,
                  .EmissiveTexture                       = GetTextureID(Material.EmissiveTexture),
                  .EmissiveTextureTexCoordIndex          = Material.EmissiveTexture.TexCoordIndex,
                  .SkyboxTexture                         = CCubemap::INVALID_VALUE,
                  .BaseColorFactor                       = Material.BaseColorFactor,
                  .EmissiveFactor                        = Material.EmissiveFactor,
                  .MetallicFactor                        = Material.MetallicFactor,
                  .RoughnessFactor                       = Material.RoughnessFactor,
                  .AlphaCutoff                           = Material.AlphaCutoff,
                  .AlphaMode                             = Material.AlphaMode,
                  .IsDoubleSided                         = Material.IsDoubleSided,
              },
          .VAO           = Primitive.VAO,
          .ModelMatrix   = TransformComponent.WorldMatrix * Primitive.PrimitiveMatrix,
          .IndicesCount  = Primitive.IndicesCount,
          .IndexType     = Primitive.Type,
          .PrimitiveMode = Primitive.Mode,
      };

      _Queue.Push(std::move(Command));
    }
  }
}

} // namespace ecs