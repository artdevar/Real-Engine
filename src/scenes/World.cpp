#include "pch.h"

#include "World.h"
#include "ecs/Components.h"
#include "ecs/systems/LightingSystem.h"
#include "ecs/systems/PhysicsSystem.h"
#include "ecs/systems/SkyboxRenderSystem.h"
#include "ecs/systems/ModelRenderSystem.h"
#include "ecs/systems/CollisionRenderSystem.h"
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
  SubscribeToEvents();
}

void CWorld::Shutdown()
{
  m_EntitiesCoordinator.reset();
}

void CWorld::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::Editor_EntitySelected:
    m_EntitiesCoordinator->GetSystem<ecs::CCollisionRenderSystem>()->OnEntitySelected(_Event.GetValue<ecs::TEntity>());
    break;
  case TEventType::Editor_EntityDeselected:
    m_EntitiesCoordinator->GetSystem<ecs::CCollisionRenderSystem>()->OnEntityDeselected(_Event.GetValue<ecs::TEntity>());
    break;
  }
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
  m_EntitiesCoordinator->GetSystem<ecs::CCollisionRenderSystem>()->Collect(_Queue);
}

ecs::TNameComponent *CWorld::GetEntityName(ecs::TEntity _Entity) const
{
  if (m_EntitiesCoordinator->DoesComponentExist<ecs::TNameComponent>(_Entity))
    return &m_EntitiesCoordinator->GetComponent<ecs::TNameComponent>(_Entity);

  return nullptr;
}

ecs::TTransformComponent *CWorld::GetTransform(ecs::TEntity _Entity) const
{
  if (m_EntitiesCoordinator->DoesComponentExist<ecs::TTransformComponent>(_Entity))
    return &m_EntitiesCoordinator->GetComponent<ecs::TTransformComponent>(_Entity);

  return nullptr;
}

ecs::TEntity CWorld::CloneEntity(ecs::TEntity _Entity)
{
  return m_EntitiesCoordinator->CloneEntity(_Entity);
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
  m_EntitiesCoordinator->RegisterComponent<ecs::TNameComponent>();
  m_EntitiesCoordinator->RegisterComponent<ecs::TCollisionComponent>();

  m_EntitiesCoordinator->RegisterSystem<ecs::CLightingSystem>();
  m_EntitiesCoordinator->RegisterSystem<ecs::CModelRenderSystem>();
  m_EntitiesCoordinator->RegisterSystem<ecs::CSkyboxRenderSystem>();
  m_EntitiesCoordinator->RegisterSystem<ecs::CPhysicsSystem>();
  m_EntitiesCoordinator->RegisterSystem<ecs::CCollisionRenderSystem>();

  {
    ecs::TSignature LightingSystemSignature;
    LightingSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TNameComponent>());
    LightingSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TLightComponent>());
    m_EntitiesCoordinator->SetSystemSignature<ecs::CLightingSystem>(LightingSystemSignature);
  }

  {
    ecs::TSignature ModelRenderSystemSignature;
    ModelRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TNameComponent>());
    ModelRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TModelComponent>());
    ModelRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TTransformComponent>());
    ModelRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TCollisionComponent>());
    m_EntitiesCoordinator->SetSystemSignature<ecs::CModelRenderSystem>(ModelRenderSystemSignature);
  }

  {
    ecs::TSignature SkyboxRenderSystemSignature;
    SkyboxRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TNameComponent>());
    SkyboxRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TSkyboxComponent>());
    m_EntitiesCoordinator->SetSystemSignature<ecs::CSkyboxRenderSystem>(SkyboxRenderSystemSignature);
  }

  {
    ecs::TSignature PhysicsSystemSignature;
    PhysicsSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TNameComponent>());
    PhysicsSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TTransformComponent>());
    m_EntitiesCoordinator->SetSystemSignature<ecs::CPhysicsSystem>(PhysicsSystemSignature);
  }

  {
    ecs::TSignature CollisionRenderSystemSignature;
    CollisionRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TTransformComponent>());
    CollisionRenderSystemSignature.set(m_EntitiesCoordinator->GetComponentType<ecs::TCollisionComponent>());
    m_EntitiesCoordinator->SetSystemSignature<ecs::CCollisionRenderSystem>(CollisionRenderSystemSignature);
  }
}

void CWorld::SubscribeToEvents()
{
  event::Subscribe(TEventType::Editor_EntitySelected, GetWeakPtr());
  event::Subscribe(TEventType::Editor_EntityDeselected, GetWeakPtr());
}