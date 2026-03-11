#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include "render/Buffer.h"
#include <common/Sharable.h>
#include <memory>

class CShader;

class COutputRenderPass : public CSharable<COutputRenderPass>,
                          public IRenderPass
{
public:
  template <typename... Args>
  static TSharedPtr Create(Args &&..._Args)
  {
    return CSharable::Create(std::forward<Args>(_Args)...);
  }

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
  using CSharable<COutputRenderPass>::Create;

  std::shared_ptr<CShader> m_Shader;
  CVertexArray             m_VAO;
  CVertexBuffer            m_VBO;
};
