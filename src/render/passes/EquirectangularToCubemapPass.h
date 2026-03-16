#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include "render/RenderContext.h"
#include "render/Buffer.h"
#include <common/Sharable.h>
#include <glm/glm.hpp>
#include <vector>

class CShader;
class CCubemap;

class CEquirectangularToCubemapPass : public CSharable<CEquirectangularToCubemapPass>,
                                      public IRenderPass
{
public:
  explicit CEquirectangularToCubemapPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::EquirectangularToCubemap;
  }

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;
  bool NeedsCommands() const override;

private:
  static constexpr inline int CUBEMAP_FACES = 6;

  std::shared_ptr<CShader>      m_Shader;
  std::shared_ptr<CVertexArray> m_VAO;

  CFrameBuffer  m_FBO;
  CRenderBuffer m_RBO;

  glm::mat4 m_Projection;
  glm::mat4 m_Views[CUBEMAP_FACES];
};
