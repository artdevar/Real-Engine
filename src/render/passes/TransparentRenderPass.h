#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include "render/RenderContext.h"
#include <common/Sharable.h>

class CShader;

class CTransparentRenderPass : public CSharable<CTransparentRenderPass>,
                               public IRenderPass
{
public:
  explicit CTransparentRenderPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::Transparent;
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