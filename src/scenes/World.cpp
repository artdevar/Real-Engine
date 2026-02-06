#include "World.h"
#include "engine/Engine.h"
#include "Shared.h"
#include "ecs/Coordinator.h"
#include "ecs/CommonECS.h"
#include "ecs/Components.h"
#include "ecs/Systems.h"
#include "ecs/EntityBuilder.h"
#include "graphics/Shader.h"
#include "graphics/Renderer.h"
#include <nlohmann/json.hpp>

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

void CWorld::Update(float _TimeDelta)
{
  m_EntitiesCoordinator->GetSystem<ecs::CPhysicsSystem>()->Update(_TimeDelta);
}

void CWorld::Render(CRenderer &_Renderer)
{
  TShaderLighting ShaderLight = m_EntitiesCoordinator->GetSystem<ecs::CLightingSystem>()->ComposeLightingData();

  m_EntitiesCoordinator->GetSystem<ecs::CSkyboxRenderSystem>()->Render(_Renderer);

  _Renderer.SetLightingData(std::move(ShaderLight));
  m_EntitiesCoordinator->GetSystem<ecs::CModelRenderSystem>()->Render(_Renderer);
}

void CWorld::RemoveEntity(ecs::TEntity _Entity)
{
  m_EntitiesCoordinator->DestroyEntity(_Entity);
}

const std::vector<ecs::TEntity> &CWorld::GetAllEntities() const
{
  return m_EntitiesCoordinator->GetExistingEntities();
}

CEntityBuilder CWorld::GetEntityBuilder() const
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