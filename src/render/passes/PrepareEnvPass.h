#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include "render/RenderContext.h"
#include "render/Buffer.h"
#include <common/Sharable.h>
#include <glm/glm.hpp>

class CShader;

class CPrepareEnvPass : public CSharable<CPrepareEnvPass>,
                        public IRenderPass
{
public:
  explicit CPrepareEnvPass();
  ERenderPassType GetType() const override
  {
    return ERenderPassType::PrepareEnvironment;
  }

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;
  bool NeedsCommands() const override;

private:
  void ConvertEquirectangularToCubemap(IRenderer &_Renderer, TRenderContext &_RenderContext, const TRenderCommand &_Command);
  void IrradianceConvolution(IRenderer &_Renderer, TRenderContext &_RenderContext, const TRenderCommand &_Command);
  void CreatePrefilterMap(IRenderer &_Renderer, TRenderContext &_RenderContext, const TRenderCommand &_Command);
  void CreateBRDFLUT(IRenderer &_Renderer, TRenderContext &_RenderContext, const TRenderCommand &_Command);

private:
  static constexpr inline int CUBEMAP_FACES       = 6;
  static constexpr inline int CUBEMAP_SIZE        = 512;
  static constexpr inline int IRRADIANCE_MAP_SIZE = 32;
  static constexpr inline int MIP_LEVELS          = 5;
  static constexpr inline int BRDF_LUT_SIZE       = 512;

  std::shared_ptr<CShader> m_EquirectToCubemapShader;
  std::shared_ptr<CShader> m_IrradianceShader;
  std::shared_ptr<CShader> m_PrefilterShader;
  std::shared_ptr<CShader> m_BRDFLUTShader;

  CFrameBuffer  m_FBO;
  CRenderBuffer m_RBO;

  glm::mat4 m_Projection;
  glm::mat4 m_Views[CUBEMAP_FACES];
};
