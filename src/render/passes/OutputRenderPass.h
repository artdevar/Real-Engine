#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include <common/Sharable.h>
#include <memory>

class CShader;

class COutputRenderPass : public CSharable<COutputRenderPass>,
                          public IRenderPass
{
public:
  explicit COutputRenderPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::Output;
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
