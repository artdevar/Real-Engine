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

namespace ecs
{

CModelRenderSystem::CModelRenderSystem() :
  m_VBO(GL_DYNAMIC_DRAW),
  m_EBO(GL_DYNAMIC_DRAW)
{
}

void CModelRenderSystem::Init(CCoordinator * _Coordinator)
{
  CSystem::Init(_Coordinator);

  m_ModelShader = CEngine::Instance()->GetResourceManager()->LoadShader("../shaders/model");
}

void CModelRenderSystem::Render(CRenderer & _Renderer)
{
  _Renderer.SetShader(m_ModelShader);
  m_ModelShader->SetUniform("u_View", _Renderer.GetCamera()->GetView());

  unsigned int offsetIndices = 0;
  unsigned int offsetVertices = 0;

  m_VAO.Bind();
  for (ecs::TEntity Entity : m_Entities)
  {
    auto & TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
    auto & ModelComponent     = m_Coordinator->GetComponent<TModelComponent>(Entity);

    m_ModelShader->SetUniform("u_Model", TransformComponent.Transform);

    GLint TextureIndex = 0;

    const TMaterial & Material = ModelComponent.Model->Meshes[0].Material;
    if (Material.DiffuseTexture)
    {
      Material.DiffuseTexture->Bind(GL_TEXTURE0 + TextureIndex);
      m_ModelShader->SetUniform("u_Material.DiffuseTexture", TextureIndex);
      TextureIndex++;
    }

    if (Material.SpecularTexture)
    {
      Material.SpecularTexture->Bind(GL_TEXTURE0 + TextureIndex);
      m_ModelShader->SetUniform("u_Material.SpecularTexture", TextureIndex);
      TextureIndex++;
    }

    m_ModelShader->SetUniform("u_Material.Shininess", Material.Shininess);

    glDrawElementsBaseVertex(GL_TRIANGLES, ModelComponent.indices, GL_UNSIGNED_INT, (void *)(offsetIndices * sizeof(unsigned int)), offsetVertices);
    glBindTexture(GL_TEXTURE_2D, 0);

    offsetVertices += ModelComponent.vertices;
    offsetIndices  += ModelComponent.indices;
  }
  m_VAO.Unbind();
}

void CModelRenderSystem::OnEntityAdded(ecs::TEntity _Entity)
{
  auto & ModelComponent = m_Coordinator->GetComponent<TModelComponent>(_Entity);

  std::vector<shared::TVertex> Vertices = ModelComponent.Model->Meshes[0].Vertices;
  std::vector<unsigned int>    Indices  = ModelComponent.Model->Meshes[0].Indices;

  //for (const auto & Mesh : ModelComponent.Model->Meshes)
  //{
  //  Vertices.insert(Vertices.end(), Mesh.Vertices.begin(), Mesh.Vertices.end());
  //  Indices.insert(Indices.end(), Mesh.Indices.begin(), Mesh.Indices.end());
  //}

  ModelComponent.vertices = Vertices.size();
  ModelComponent.indices = Indices.size();

  m_VAO.Bind();

  m_VBO.Bind();
  m_VBO.Push(Vertices);

  m_EBO.Bind();
  m_EBO.Push(Indices);

  m_VAO.EnableVertexAttrib(0, 3, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, Position));
  m_VAO.EnableVertexAttrib(1, 3, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, Normal));
  m_VAO.EnableVertexAttrib(2, 2, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, TexCoords));
  m_VAO.EnableVertexAttrib(3, 3, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, Tangent));
  m_VAO.EnableVertexAttrib(4, 3, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, Bitangent));

  m_VAO.Unbind();
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
  m_VBO.Assign(shared::SkyboxVertices, ARRAY_SIZE(shared::SkyboxVertices));

  m_VAO.EnableVertexAttrib(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

  m_VBO.Unbind();
  m_VAO.Unbind();
}

void CSkyboxRenderSystem::Render(CRenderer &_Renderer)
{
  _Renderer.SetShader(m_SkyboxShader);
  m_SkyboxShader->SetUniform("u_View", glm::mat4(glm::mat3(_Renderer.GetCamera()->GetView())));

  glDepthFunc(GL_LEQUAL);

  assert(m_Entities.size() <= 1); // it isn't supposed to be more than 1 skybox

  m_VAO.Bind();
  for (ecs::TEntity Entity : m_Entities)
  {
    auto & SkyboxComponent = m_Coordinator->GetComponent<TSkyboxComponent>(Entity);

    const int TextureIndex = 0;
    SkyboxComponent.Cubemap->Bind(GL_TEXTURE0 + TextureIndex);
    m_SkyboxShader->SetUniform("u_Cubemap", TextureIndex);

    _Renderer.DrawArrays(GL_TRIANGLES, ARRAY_SIZE(shared::SkyboxVertices) / 3);

    SkyboxComponent.Cubemap->Unbind();
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
  MEM_ZERO(Lighting);

  int PointLightIndex = 0;

  for (auto Entity : m_Entities)
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

// Shutdown

void CShutdownSystem::Shutdown()
{

}

}