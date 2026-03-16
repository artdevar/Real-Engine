#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include "render/RenderContext.h"
#include "render/Buffer.h"
#include <common/Sharable.h>
#include <glm/glm.hpp>

class CShader;

class CIrradianceConvolutionPass : public CSharable<CIrradianceConvolutionPass>,
                                   public IRenderPass
{
public:
  explicit CIrradianceConvolutionPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::IrradianceConvolution;
  }

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;
  bool NeedsCommands() const override;

private:
  static constexpr inline int CUBEMAP_FACES       = 6;
  static constexpr inline int IRRADIANCE_MAP_SIZE = 32;

  std::shared_ptr<CShader> m_Shader;

  CFrameBuffer  m_FBO;
  CRenderBuffer m_RBO;

  glm::mat4 m_Projection;
  glm::mat4 m_Views[CUBEMAP_FACES];
};
