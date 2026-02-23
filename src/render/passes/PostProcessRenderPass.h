#pragma once

#include "interfaces/RenderPass.h"
#include "render/Buffer.h"

class CShader;

class PostProcessRenderPass : public IRenderPass
{
public:
  explicit PostProcessRenderPass(std::shared_ptr<CShader> _Shader);

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;

private:
  std::weak_ptr<CShader> m_Shader;

  CVertexArray  m_VAO;
  CVertexBuffer m_VBO;
};