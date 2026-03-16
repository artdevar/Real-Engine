#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include <common/Sharable.h>

class CShader;

class CSkyboxRenderPass : public CSharable<CSkyboxRenderPass>,
                          public IRenderPass
{
public:
  explicit CSkyboxRenderPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::Skybox;
  }

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;
  bool NeedsCommands() const override;

private:
  std::shared_ptr<CShader> m_Shader;
};
