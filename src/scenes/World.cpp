#include "pch.h"

#include "World.h"
#include "ecs/CommonECS.h"
#include "ecs/Components.h"
#include "ecs/Coordinator.h"
#include "ecs/EntityBuilder.h"
#include "ecs/systems/LightingSystem.h"
#include "ecs/systems/PhysicsSystem.h"
#include "ecs/systems/SkyboxRenderSystem.h"
#include "ecs/systems/ModelRenderSystem.h"
#include "engine/Engine.h"
#include "assets/Shader.h"
#include "render/RenderQueue.h"

CWorld::CWorld() = default;

CWorld::~CWorld() = default;

void CWorld::Init()
{
  InitECS();
}

void CWorld::Shutdown()
{
  m_EntitiesCoordinator.reset();
}

void CWorld::UpdateInternal(float _TimeDelta)
{
  m_EntitiesCoordinator->GetSystem<ecs::CPhysicsSystem>()->Update(_TimeDelta);
}

bool CWorld::ShouldBeUpdated() const
{
  return m_EntitiesCoordinator != nullptr;
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

void CWorld::RemoveEntity(ecs::TEntity _Entity)
{
  m_EntitiesCoordinator->DestroyEntity(_Entity);
}

const CUnorderedVector<ecs::TEntity> &CWorld::GetAllEntities() const
{
  return m_EntitiesCoordinator->GetExistingEntities();
}

CEntityBuilder CWorld::CreateEntity()
{
  return CEntityBuilder(m_EntitiesCoordinator.get());
}

void CWorld::InitECS()
{
  m_EntitiesCoordinator = std::make_unique<CCoordinator>();

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
