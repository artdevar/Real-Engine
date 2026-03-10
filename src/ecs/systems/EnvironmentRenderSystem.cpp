#include "pch.h"

#include "EnvironmentRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "utils/Resource.h"
#include "render/RenderCommand.h"
#include "render/RenderQueue.h"
#include "render/FrameData.h"
#include "assets/Texture.h"

namespace ecs
{

void CEnvironmentRenderSystem::Collect(TFrameData &_FrameData)
{
  if (m_Entities.Empty())
    return;

  ecs::TEntity Entity          = m_Entities[0];
  auto        &SkyboxComponent = m_Coordinator->GetComponent<TEnvironmentComponent>(Entity);

  _FrameData.Environment = TFrameData::TEnvironment{
      .SkyboxTexture      = SkyboxComponent.SkyboxTexture ? SkyboxComponent.SkyboxTexture->ID() : CCubemap::INVALID_VALUE,
      .EquirectangularMap = SkyboxComponent.EquirectangularMap ? SkyboxComponent.EquirectangularMap->ID() : CTexture::INVALID_VALUE,
      .IrradianceMap      = SkyboxComponent.IrradianceMap ? SkyboxComponent.IrradianceMap->ID() : CCubemap::INVALID_VALUE,
  };
}

void CEnvironmentRenderSystem::Collect(CRenderQueue &_Queue)
{
  if (m_Entities.Empty())
    return;

  ecs::TEntity Entity          = m_Entities[0];
  auto        &SkyboxComponent = m_Coordinator->GetComponent<TEnvironmentComponent>(Entity);

  const uint32_t Skybox             = SkyboxComponent.SkyboxTexture ? SkyboxComponent.SkyboxTexture->ID() : CCubemap::INVALID_VALUE;
  const uint32_t IrradianceMap      = SkyboxComponent.IrradianceMap ? SkyboxComponent.IrradianceMap->ID() : CCubemap::INVALID_VALUE;
  const uint32_t EquirectangularMap = SkyboxComponent.EquirectangularMap ? SkyboxComponent.EquirectangularMap->ID() : CTexture::INVALID_VALUE;

  switch (m_ConversionStage)
  {
  case EConversionStage::WaitingForEquirectangular: {
    TRenderFlags RenderFlags;
    RenderFlags.set(ERenderFlags_EquirectangularToCubemap);

    TRenderCommand Command{
        .Environment =
            TEnvironment{
                .SkyboxTexture      = Skybox,
                .EquirectangularMap = EquirectangularMap,
                .IrradianceMap      = IrradianceMap,
            },
        .ModelMatrix = glm::mat4(1.0f),
        .RenderFlags = std::move(RenderFlags),
    };

    _Queue.Push(std::move(Command));
    m_ConversionStage = EConversionStage::EquirectangularConverted;
    break;
  }
  case EConversionStage::EquirectangularConverted: {
    TRenderFlags RenderFlags;
    RenderFlags.set(ERenderFlags_IrradianceConvolution);

    TRenderCommand Command{
        .Environment =
            TEnvironment{
                .SkyboxTexture      = Skybox,
                .EquirectangularMap = EquirectangularMap,
                .IrradianceMap      = IrradianceMap,
            },
        .ModelMatrix = glm::mat4(1.0f),
        .RenderFlags = std::move(RenderFlags),
    };

    _Queue.Push(std::move(Command));
    m_ConversionStage = EConversionStage::IrradianceConverted;
    break;
  }
  case EConversionStage::IrradianceConverted:
    [[likely]]
    {
      TRenderFlags RenderFlags;
      RenderFlags.set(ERenderFlags_Skybox);

      TRenderCommand Command{
          .Environment =
              TEnvironment{
                  .SkyboxTexture = Skybox,
                  .IrradianceMap = IrradianceMap,
              },
          .ModelMatrix = glm::mat4(1.0f),
          .RenderFlags = std::move(RenderFlags),
      };

      _Queue.Push(std::move(Command));
      break;
    }
  }
}

void CEnvironmentRenderSystem::OnEntityAdded(ecs::TEntity _Entity)
{
  assert(m_Entities.Size() == 1 && "It isn't supposed to be more than 1 environment");
  m_ConversionStage = EConversionStage::WaitingForEquirectangular;
}

} // namespace ecs
