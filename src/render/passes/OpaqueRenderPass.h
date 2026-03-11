#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include <common/Sharable.h>

class CShader;

class COpaqueRenderPass : public CSharable<COpaqueRenderPass>,
                          public IRenderPass
{
public:
  explicit COpaqueRenderPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::Opaque;
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