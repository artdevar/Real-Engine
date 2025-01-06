#include "Systems.h"
#include "Components.h"
#include "Coordinator.h"
#include "graphics/Shader.h"
#include "graphics/ShaderTypes.h"
#include "graphics/Renderer.h"
#include "graphics/Model.h"
#include "engine/Engine.h"
#include "engine/ResourceManager.h"
#include "engine/Camera.h"
#include "utils/Common.h"
#include <glm/glm.hpp>


#include "tiny_gltf.h"

namespace ecs
{

CModelRenderSystem::CModelRenderSystem()
{
}

void CModelRenderSystem::Init(CCoordinator * _Coordinator)
{
  CSystem::Init(_Coordinator);

  m_ModelShader = CEngine::Instance()->GetResourceManager()->LoadShader("../shaders/model");
}

void CModelRenderSystem::Render(CRenderer & _Renderer)
{
  if (m_Entities.Empty())
    return;

  std::shared_ptr<CShader> Shader = m_ModelShader.lock();
  _Renderer.SetShader(Shader);

  for (ecs::TEntity Entity : m_Entities)
  {
    auto & TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
    auto & ModelComponent     = m_Coordinator->GetComponent<TModelComponent>(Entity);

    Shader->SetUniform("u_Model", TransformComponent.Transform);

    for (TModelComponent::TPrimitiveData & Primitive : ModelComponent.Primitives)
    {
      Primitive.VAO.Bind();
      glDrawElements(GL_TRIANGLES, Primitive.Indices, GL_UNSIGNED_INT, (int8_t*)0 + Primitive.Offset);
      Primitive.VAO.Unbind();
    }
  }
}

void CModelRenderSystem::SetVisibility(ecs::TEntity _Entity, bool _IsVisible)
{
  if (_IsVisible)
  {
    assert(m_HiddenEntities.Contains(_Entity));
    assert(!m_Entities.Contains(_Entity));

    m_Entities.Push(_Entity);
    m_HiddenEntities.Erase(_Entity);
  }
  else
  {
    assert(!m_HiddenEntities.Contains(_Entity));
    assert(m_Entities.Contains(_Entity));

    m_Entities.Erase(_Entity);
    m_HiddenEntities.Push(_Entity);
  }
}

void CModelRenderSystem::OnEntityAdded(ecs::TEntity _Entity)
{
  CSystem::OnEntityAdded(_Entity);

}

void CModelRenderSystem::OnEntityDeleted(ecs::TEntity _Entity)
{
  CSystem::OnEntityDeleted(_Entity);

  m_HiddenEntities.SafeErase(_Entity);
}

// Skybox

CSkyboxRenderSystem::CSkyboxRenderSystem() : m_VBO(GL_STATIC_DRAW)
{
}

void CSkyboxRenderSystem::Init(CCoordinator *_Coordinator)
{
  CSystem::Init(_Coordinator);

  m_SkyboxShader = CEngine::Instance()->GetResourceManager()->LoadShader("../shaders/skybox");

  m_VAO.Bind();

  m_VBO.Bind();
  m_VBO.Assign(shared::SkyboxVertices, sizeof(shared::SkyboxVertices));

  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, 3 * sizeof(float), (GLvoid*)0);

  m_VAO.Unbind();
}

void CSkyboxRenderSystem::Render(CRenderer & _Renderer)
{
  if (m_Entities.Empty())
    return;

  std::shared_ptr<CShader> Shader = m_SkyboxShader.lock();

  _Renderer.SetShader(Shader);
  Shader->SetUniform("u_View", glm::mat4(glm::mat3(_Renderer.GetCamera()->GetView())));

  glDepthFunc(GL_LEQUAL);

  assert(m_Entities.Size() <= 1); // it isn't supposed to be more than 1 skybox

  m_VAO.Bind();
  for (ecs::TEntity Entity : m_Entities)
  {
    auto & SkyboxComponent = m_Coordinator->GetComponent<TSkyboxComponent>(Entity);

    const int TextureIndex = 0;
    glActiveTexture(GL_TEXTURE0 + TextureIndex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxComponent.TextureUnit);
    Shader->SetUniform("u_Cubemap", TextureIndex);

    _Renderer.DrawArrays(GL_TRIANGLES, ARRAY_SIZE(shared::SkyboxVertices) / 3);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  }
  m_VAO.Unbind();

  glDepthFunc(GL_LESS);
}

// Physics

void CPhysicsSystem::Update(float _TimeDelta)
{

}

// Lighting

TShaderLighting CLightingSystem::ComposeLightingData() const
{
  TShaderLighting Lighting;
  FastMemSet(&Lighting, 0x00, sizeof(Lighting));

  int PointLightIndex = 0;

  for (ecs::TEntity Entity : m_Entities)
  {
    auto & Light = m_Coordinator->GetComponent<TLightComponent>(Entity);

    switch (Light.Type)
    {
      case ELightType::Point:
        Lighting.LightPoints[PointLightIndex].Ambient   = Light.Ambient;
        Lighting.LightPoints[PointLightIndex].Position  = Light.Position;
        Lighting.LightPoints[PointLightIndex].Diffuse   = Light.Diffuse;
        Lighting.LightPoints[PointLightIndex].Specular  = Light.Specular;
        Lighting.LightPoints[PointLightIndex].Constant  = Light.Constant;
        Lighting.LightPoints[PointLightIndex].Linear    = Light.Linear;
        Lighting.LightPoints[PointLightIndex].Quadratic = Light.Quadratic;
        Lighting.PointLightsCount = PointLightIndex++;
        break;

      case ELightType::Directional:
        Lighting.LightDirectional.Ambient   = Light.Ambient;
        Lighting.LightDirectional.Diffuse   = Light.Diffuse;
        Lighting.LightDirectional.Specular  = Light.Specular;
        Lighting.LightDirectional.Direction = Light.Direction;
        break;

      case ELightType::Spotlight:
        Lighting.LightSpot.Ambient     = Light.Ambient;
        Lighting.LightSpot.Position    = Light.Position;
        Lighting.LightSpot.Direction   = Light.Direction;
        Lighting.LightSpot.Diffuse     = Light.Diffuse;
        Lighting.LightSpot.Specular    = Light.Specular;
        Lighting.LightSpot.CutOff      = Light.CutOff;
        Lighting.LightSpot.OuterCutOff = Light.OuterCutOff;
        Lighting.LightSpot.Constant    = Light.Constant;
        Lighting.LightSpot.Linear      = Light.Linear;
        Lighting.LightSpot.Quadratic   = Light.Quadratic;
        break;

      default:
        assert(false);
        break;
    }
  }

  return Lighting;
}

}