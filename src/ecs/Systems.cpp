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

namespace ecs
{

CModelRenderSystem::CModelRenderSystem() :
  m_VBO(GL_DYNAMIC_DRAW),
  m_MBO(GL_DYNAMIC_DRAW),
  m_EBO(GL_DYNAMIC_DRAW),
  m_IBO(GL_DYNAMIC_DRAW),
  m_SBO(GL_DYNAMIC_DRAW)
{
}

void CModelRenderSystem::Init(CCoordinator * _Coordinator)
{
  CSystem::Init(_Coordinator);

  m_ModelShader = CEngine::Instance()->GetResourceManager()->LoadShader("../shaders/indir");
}

void CModelRenderSystem::Render(CRenderer & _Renderer)
{
  if (m_Entities.empty())
    return;

  std::shared_ptr<CShader> Shader = m_ModelShader.lock();

  _Renderer.SetShader(Shader);

#if 0

  unsigned int offsetIndices = 0;
  unsigned int offsetVertices = 0;

  m_VAO.Bind();
  for (ecs::TEntity Entity : m_Entities)
  {
    auto & TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
    auto & ModelComponent     = m_Coordinator->GetComponent<TModelComponent>(Entity);

    m_ModelShader->SetUniform("u_Model", TransformComponent.Transform);

    GLint TextureIndex = 0;

    const TMaterial & Material = ModelComponent.Model->GetMeshes()[0].Material;
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

#endif

  m_VAO.Bind();

  m_MBO.Bind();
  m_MBO.Clear();
  m_MBO.Reserve(m_Entities.size() * sizeof(TTransformComponent::Transform));

  std::vector<TDrawCommand> DrawCommands;
  std::vector<GLuint64>     TextureHandles;

  GLuint OffsetIndices  = 0;
  GLuint OffsetVertices = 0;
  GLuint Index          = 0;

  for (auto & [Entity, BufferData] : m_EntityBufferData)
  {
    auto & TransformComponent = m_Coordinator->GetComponent<TTransformComponent>(Entity);
    auto & ModelComponent     = m_Coordinator->GetComponent<TModelComponent>(Entity);

    m_MBO.Push(glm::value_ptr(TransformComponent.Transform), sizeof(TransformComponent.Transform));

    const TMaterial & Material = ModelComponent.Model->GetMeshes()[0].Material;
    auto thandle = glGetTextureHandleARB(Material.DiffuseTexture->Get());
    assert(thandle != 0);
    TextureHandles.push_back(thandle);

    TDrawCommand & DrawCommand = DrawCommands.emplace_back();

    DrawCommand.Elements     = BufferData.Indices;
    DrawCommand.Instances    = 1;
    DrawCommand.FirstIndex   = OffsetIndices;
    DrawCommand.BaseVertex   = OffsetVertices;
    DrawCommand.BaseInstance = Index++;

    OffsetVertices += BufferData.Vertices;
    OffsetIndices  += BufferData.Indices;
  }

  m_VAO.EnableAttribWithDivisor(ATTRIB_LOC_TRANSFORM + 0, 4, GL_FLOAT, sizeof(glm::mat4x4), (GLvoid*)0,  1);
  m_VAO.EnableAttribWithDivisor(ATTRIB_LOC_TRANSFORM + 1, 4, GL_FLOAT, sizeof(glm::mat4x4), (GLvoid*)16, 1);
  m_VAO.EnableAttribWithDivisor(ATTRIB_LOC_TRANSFORM + 2, 4, GL_FLOAT, sizeof(glm::mat4x4), (GLvoid*)32, 1);
  m_VAO.EnableAttribWithDivisor(ATTRIB_LOC_TRANSFORM + 3, 4, GL_FLOAT, sizeof(glm::mat4x4), (GLvoid*)48, 1);

  m_IBO.Bind();
  m_IBO.Assign(DrawCommands);
  m_IBO.BindToTarget(GL_ARRAY_BUFFER); // so vertex attrib will work
  m_VAO.EnableAttribWithDivisor(ATTRIB_LOC_DRAW_ID, 1, GL_UNSIGNED_INT, STRIDE_AND_OFFSET(TDrawCommand, BaseInstance), 1);

  m_SBO.Bind();
  m_SBO.Assign(TextureHandles);
  m_SBO.BindToBase(BINDING_TEXTURE_BUFFER);

  //for (GLuint64 t : TextureHandles)
  //  glMakeTextureHandleResidentARB(t);

  glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, m_Entities.size(), 0);

  //for (auto t : TextureHandles)
  //  glMakeTextureHandleNonResidentARB(t);

  m_VAO.Unbind();
}

void CModelRenderSystem::OnEntityAdded(ecs::TEntity _Entity)
{
  CSystem::OnEntityAdded(_Entity);

  auto & ModelComponent = m_Coordinator->GetComponent<TModelComponent>(_Entity);

  const TMesh & MESH = ModelComponent.Model->GetMeshes()[0];

  std::vector<shared::TVertex> Vertices = MESH.Vertices;
  std::vector<unsigned int>    Indices  = MESH.Indices;

  //for (const auto & m : ModelComponent.Model->GetMeshes())
  //{
  //  Vertices.insert(Vertices.end(), m.Vertices.begin(), m.Vertices.end());
  //  Indices.insert(Indices.end(), m.Indices.begin(), m.Indices.end());
  //}

  assert(!m_EntityBufferData.contains(_Entity));

  TEntityBufferData  EntityBufferData;
  EntityBufferData.VBOOffset = m_VBO.GetSize();
  EntityBufferData.VBOSize   = Vertices.size() * sizeof(shared::TVertex);
  EntityBufferData.EBOOffset = m_EBO.GetSize();
  EntityBufferData.EBOSize   = Indices.size() * sizeof(unsigned int);
  EntityBufferData.Vertices  = Vertices.size();
  EntityBufferData.Indices   = Indices.size();

  m_EntityBufferData.emplace(_Entity, EntityBufferData);

  m_VAO.Bind();

  m_VBO.Bind();
  m_VBO.Push(Vertices);
  OnVBOContentChanged();

  m_EBO.Bind();
  m_EBO.Push(Indices);

  m_VAO.Unbind();
}

void CModelRenderSystem::OnEntityDeleted(ecs::TEntity _Entity)
{
  CSystem::OnEntityDeleted(_Entity);

  const auto DataIter = m_EntityBufferData.find(_Entity);
  assert(DataIter != m_EntityBufferData.end());

  const TEntityBufferData DeletedEntityData = DataIter->second;
  m_EntityBufferData.erase(DataIter);

  m_VAO.Bind();

  m_VBO.Bind();
  m_VBO.Erase(DeletedEntityData.VBOOffset, DeletedEntityData.VBOSize);
  OnVBOContentChanged();

  m_EBO.Bind();
  m_EBO.Erase(DeletedEntityData.EBOOffset, DeletedEntityData.EBOSize);

  for (auto & [Entity, BufferData] : m_EntityBufferData)
  {
    if (BufferData.VBOOffset > DeletedEntityData.VBOOffset)
      BufferData.VBOOffset -= DeletedEntityData.VBOSize;

    if (BufferData.EBOOffset > DeletedEntityData.EBOOffset)
      BufferData.EBOOffset -= DeletedEntityData.EBOSize;
  }

  m_VAO.Unbind();
}

void CModelRenderSystem::OnVBOContentChanged()
{
  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION,  3, GL_FLOAT, STRIDE_AND_OFFSET(shared::TVertex, Position));
  m_VAO.EnableAttrib(ATTRIB_LOC_NORMAL,    3, GL_FLOAT, STRIDE_AND_OFFSET(shared::TVertex, Normal));
  m_VAO.EnableAttrib(ATTRIB_LOC_TEXCOORDS, 2, GL_FLOAT, STRIDE_AND_OFFSET(shared::TVertex, TexCoords));
  m_VAO.EnableAttrib(ATTRIB_LOC_TANGENT,   3, GL_FLOAT, STRIDE_AND_OFFSET(shared::TVertex, Tangent));
  m_VAO.EnableAttrib(ATTRIB_LOC_BITANGENT, 3, GL_FLOAT, STRIDE_AND_OFFSET(shared::TVertex, Bitangent));
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
  if (m_Entities.empty())
    return;

  std::shared_ptr<CShader> Shader = m_SkyboxShader.lock();

  _Renderer.SetShader(Shader);
  Shader->SetUniform("u_View", glm::mat4(glm::mat3(_Renderer.GetCamera()->GetView())));

  glDepthFunc(GL_LEQUAL);

  assert(m_Entities.size() <= 1); // it isn't supposed to be more than 1 skybox

  m_VAO.Bind();
  for (ecs::TEntity Entity : m_Entities)
  {
    auto & SkyboxComponent = m_Coordinator->GetComponent<TSkyboxComponent>(Entity);

    const int TextureIndex = 0;
    SkyboxComponent.Cubemap->Bind(GL_TEXTURE0 + TextureIndex);
    Shader->SetUniform("u_Cubemap", TextureIndex);

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