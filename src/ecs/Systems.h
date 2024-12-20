#pragma once

#include "SystemManager.h"
#include "CommonECS.h"
#include "interfaces/Renderable.h"
#include "interfaces/Updateable.h"
#include "interfaces/Shutdownable.h"
#include "graphics/Buffer.h"
#include <map>

struct TShaderLighting;
class CShader;

namespace ecs
{

class CModelRenderSystem :
  public IRenderable,
  public CSystem
{
public:

  CModelRenderSystem();

  void Init(CCoordinator * _Coordinator) override;

  void Render(CRenderer & _Renderer) override;

protected:

  void OnEntityAdded(ecs::TEntity _Entity) override;

  void OnEntityDeleted(ecs::TEntity _Entity) override;

  void OnVBOContentChanged();

protected:

  struct TDrawCommand
  {
    GLuint Elements;
    GLuint Instances;
    GLuint FirstIndex;
    GLuint BaseVertex;
    GLuint BaseInstance;
  };

  struct TEntityBufferData
  {
    GLintptr VBOOffset;
    GLintptr EBOOffset;

    GLsizeiptr VBOSize;
    GLsizeiptr EBOSize;

    uint32_t Vertices;
    uint32_t Indices;
  };

  std::weak_ptr<CShader>               m_ModelShader;
  std::map<TEntity, TEntityBufferData> m_EntityBufferData;

  CVertexArray    m_VAO; // vertex array
  CVertexBuffer   m_VBO; // vertex buffer
  CElementBuffer  m_EBO; // indices buffer

  CVertexBuffer   m_MBO; // matrix buffer
  CIndirectBuffer m_IBO; // draw commands buffer
  CShaderBuffer   m_SBO; // textures buffer
};

// --------------------------------------------------

class CSkyboxRenderSystem :
  public IRenderable,
  public CSystem
{
public:

  CSkyboxRenderSystem();

  void Init(CCoordinator * _Coordinator) override;

  void Render(CRenderer & _Renderer) override;

protected:

  std::weak_ptr<CShader> m_SkyboxShader;
  CVertexArray           m_VAO;
  CVertexBuffer          m_VBO;
};

// --------------------------------------------------

class CPhysicsSystem :
  public IUpdateable,
  public CSystem
{
public:

  void Update(float _TimeDelta) override;
};

// --------------------------------------------------

class CLightingSystem :
  public CSystem
{
public:

  TShaderLighting ComposeLightingData() const;
};

}