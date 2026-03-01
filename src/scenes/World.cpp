#include "pch.h"

#include "World.h"
#include "ecs/Components.h"
#include "ecs/systems/LightingSystem.h"
#include "ecs/systems/PhysicsSystem.h"
#include "ecs/systems/SkyboxRenderSystem.h"
#include "ecs/systems/ModelRenderSystem.h"
#include "engine/Engine.h"
#include "assets/Shader.h"
#include "render/RenderQueue.h"
#include "utils/Event.h"
#include <ecs/EntitySpawner.h>
#include <ecs/Coordinator.h>

CWorld::CWorld() :
    m_EntitiesCoordinator(std::make_unique<ecs::CCoordinator>())
{
}

CWorld::~CWorld() = default;

void CWorld::Init()
{
  InitECS();
}

void CWorld::Shutdown()
{
  m_EntitiesCoordinator.reset();
}

void CWorld::Update(float _TimeDelta)
{
  m_EntitiesCoordinator->GetSystem<ecs::CPhysicsSystem>()->Update(_TimeDelta);
}

void CWorld::Collect(TFrameData &_FrameData)
{
  m_EntitiesCoordinator->GetSystem<ecs::CLightingSystem>()->Collect(_FrameData);
}

void CWorld::Collect(CRenderQueue &_Queue)
{
  m_EntitiesCoordinator->GetSystem<ecs::CModelRenderSystem>()->Collect(_Queue);
  m_EntitiesCoordinator->GetSystem<ecs::CSkyboxRenderSystem>()->Collect(_Queue);
}

void CWorld::DestroyEntity(ecs::TEntity _Entity)
{
  m_EntitiesCoordinator->DestroyEntity(_Entity);
}

CUnorderedVector<ecs::TEntity> CWorld::GetEntities() const
{
  return m_EntitiesCoordinator->GetEntities();
}

ecs::CEntitySpawner CWorld::CreateEntitySpawner()
{
  return ecs::CEntitySpawner(*m_EntitiesCoordinator);
}

CUnorderedVector<ecs::TComponentView> CWorld::GetEntityComponents(ecs::TEntity _Entity) const
{
  return m_EntitiesCoordinator->GetEntityComponents(_Entity);
}

void CWorld::InitECS()
{
  m_EntitiesCoordinator->Init();

  m_EntitiesCoordinator->RegisterComponent<ecs::TModelComponent>();
  m_EntitiesCoordinator->RegisterComponent<ecs::TTransformComponent>();
  m_EntitiesCoordinator->RegisterComponent<ecs::TLightComponent>();
  m_EntitiesCoordinator->RegisterComponent<ecs::TSkyboxComponent>();

  m_EntitiesCoordinator->RegisterSystem<ecs::CLightingSystem>();
  m_EntitiesCoordinator->RegisterSystem<ecs::CModelRenderSystem>();
  m_EntitiesCoordinator->RegisterSystem<ecs::CSkyboxRenderSystem>();
  m_EntitiesCoordinator->RegisterSystem<ecs::CPhysicsSystem>();

  {
    ecs::TSignature LightingSystemSignature;
    LightingSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TLightComponent>());
    m_EntitiesCoordinator->SetSystemSignature<ecs::CLightingSystem>(LightingSystemSignature);
  }

  {
    ecs::TSignature ModelRenderSystemSignature;
    ModelRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TModelComponent>());
    ModelRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TTransformComponent>());
    m_EntitiesCoordinator->SetSystemSignature<ecs::CModelRenderSystem>(ModelRenderSystemSignature);
  }

  {
    ecs::TSignature SkyboxRenderSystemSignature;
    SkyboxRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TSkyboxComponent>());
    m_EntitiesCoordinator->SetSystemSignature<ecs::CSkyboxRenderSystem>(SkyboxRenderSystemSignature);
  }

  {
    ecs::TSignature PhysicsSystemSignature;
    PhysicsSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TTransformComponent>());
    m_EntitiesCoordinator->SetSystemSignature<ecs::CPhysicsSystem>(PhysicsSystemSignature);
  }
}
