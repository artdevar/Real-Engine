#include "pch.h"

#include "EnvironmentRenderSystem.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
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

  ecs::TEntity Entity       = m_Entities[0];
  auto        &EnvComponent = m_Coordinator->GetComponent<TEnvironmentComponent>(Entity);

  _FrameData.Environment = TFrameData::TEnvironment{
      .SkyboxTexture      = EnvComponent.SkyboxTexture ? EnvComponent.SkyboxTexture->ID() : CTexture::INVALID_TEXTURE,
      .EquirectangularMap = EnvComponent.EquirectangularMap ? EnvComponent.EquirectangularMap->ID() : CTexture::INVALID_TEXTURE,
      .IrradianceMap      = EnvComponent.IrradianceMap ? EnvComponent.IrradianceMap->ID() : CTexture::INVALID_TEXTURE,
      .PrefilteredMap     = EnvComponent.PrefilteredMap ? EnvComponent.PrefilteredMap->ID() : CTexture::INVALID_TEXTURE,
      .BRDFLUT            = EnvComponent.BRDFLUT ? EnvComponent.BRDFLUT->ID() : CTexture::INVALID_TEXTURE,
  };
}

void CEnvironmentRenderSystem::Collect(CRenderQueue &_Queue)
{
  if (m_Entities.Empty())
    return;

  ecs::TEntity Entity       = m_Entities[0];
  auto        &EnvComponent = m_Coordinator->GetComponent<TEnvironmentComponent>(Entity);

  const uint32_t Skybox             = EnvComponent.SkyboxTexture ? EnvComponent.SkyboxTexture->ID() : CTexture::INVALID_TEXTURE;
  const uint32_t IrradianceMap      = EnvComponent.IrradianceMap ? EnvComponent.IrradianceMap->ID() : CTexture::INVALID_TEXTURE;
  const uint32_t EquirectangularMap = EnvComponent.EquirectangularMap ? EnvComponent.EquirectangularMap->ID() : CTexture::INVALID_TEXTURE;
  const uint32_t PrefilteredMap     = EnvComponent.PrefilteredMap ? EnvComponent.PrefilteredMap->ID() : CTexture::INVALID_TEXTURE;
  const uint32_t BRDFLUT            = EnvComponent.BRDFLUT ? EnvComponent.BRDFLUT->ID() : CTexture::INVALID_TEXTURE;

  if (m_IsEnvironmentPrepared) [[likely]]
  {
    TRenderFlags RenderFlags;
    RenderFlags.set(ERenderFlags_Skybox);

    TRenderCommand Command{
        .Environment =
            TEnvironment{
                .SkyboxTexture      = Skybox,
                .EquirectangularMap = EquirectangularMap,
                .IrradianceMap      = IrradianceMap,
                .PrefilteredMap     = PrefilteredMap,
                .BRDFLUT            = BRDFLUT,
            },
        .ModelMatrix = glm::mat4(1.0f),
        .RenderFlags = std::move(RenderFlags),
    };

    _Queue.Push(std::move(Command));
  }
  else
  {
    m_IsEnvironmentPrepared = true;

    TRenderFlags RenderFlags;
    RenderFlags.set(ERenderFlags_PrepareEnvironment);

    TRenderCommand Command{
        .Environment =
            TEnvironment{
                .SkyboxTexture      = Skybox,
                .EquirectangularMap = EquirectangularMap,
                .IrradianceMap      = IrradianceMap,
                .PrefilteredMap     = PrefilteredMap,
                .BRDFLUT            = BRDFLUT,
            },
        .ModelMatrix = glm::mat4(1.0f),
        .RenderFlags = std::move(RenderFlags),
    };

    _Queue.Push(std::move(Command));
  }
}

void CEnvironmentRenderSystem::OnEntityAdded(ecs::TEntity _Entity)
{
  assert(m_Entities.Size() == 1 && "It isn't supposed to be more than 1 environment");
  m_IsEnvironmentPrepared = false;
}

} // namespace ecs
