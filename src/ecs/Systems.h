#pragma once

#include "SystemManager.h"
#include "interfaces/Renderable.h"
#include "interfaces/Updateable.h"
#include "interfaces/Shutdownable.h"
#include "graphics/Buffer.h"

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

protected:

  std::shared_ptr<CShader> m_ModelShader;

  CVertexArray   m_VAO;
  CVertexBuffer  m_VBO;
  CElementBuffer m_EBO;
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

  std::shared_ptr<CShader> m_SkyboxShader;
  CVertexArray             m_VAO;
  CVertexBuffer            m_VBO;
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

// --------------------------------------------------

class CShutdownSystem :
  public IShutdownable,
  public CSystem
{
public:

  void Shutdown() override;
};

}