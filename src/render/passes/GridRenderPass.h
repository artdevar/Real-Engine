#pragma once

#include "RenderPassTypes.h"
#include "interfaces/RenderPass.h"
#include "render/Buffer.h"
#include <common/Sharable.h>

class CShader;

class CGridRenderPass final : public CSharable<CGridRenderPass>,
                              public IRenderPass
{
public:
  explicit CGridRenderPass();

  ERenderPassType GetType() const override
  {
    return ERenderPassType::Grid;
  }

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const CommandsList &_Commands) override;

  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;
  bool NeedsCommands() const override;

private:
  std::shared_ptr<CShader> m_Shader;
  CVertexArray             m_VAO;
  CVertexBuffer            m_VBO;
  uint32_t                 m_VertexCount;
};
