#pragma once

#include "ecs/CommonECS.h"
#include "ecs/SystemManager.h"
#include "graphics/Buffer.h"
#include "interfaces/Renderable.h"

class CShader;

namespace ecs
{

class CSkyboxRenderSystem : public IRenderable,
                            public CSystem
{
public:
  void Init(CCoordinator *_Coordinator) override;

private:
  void RenderInternal(IRenderer &_Renderer) override;
  bool ShouldBeRendered() const override;
  void PrepareRenderState(IRenderer &_Renderer);

protected:
  std::weak_ptr<CShader> m_SkyboxShader;
};

} // namespace ecs
