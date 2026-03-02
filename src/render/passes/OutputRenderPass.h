#pragma once

#include "interfaces/RenderPass.h"
#include "interfaces/Sharable.h"
#include "render/Buffer.h"
#include <memory>

class CShader;

class COutputRenderPass : public CSharable<COutputRenderPass>,
                          public IRenderPass
{
public:
  template <typename... Args>
  static SharedPtr Create(Args &&..._Args)
  {
    return CSharable::Create(std::forward<Args>(_Args)...);
  }

public:
  explicit COutputRenderPass(std::shared_ptr<CShader> _Shader);

  void PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  void PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands) override;
  bool Accepts(const TRenderCommand &_Command) const override;
  bool IsAvailable() const override;

private:
  using CSharable<COutputRenderPass>::Create;

  std::shared_ptr<CShader> m_Shader;
  CVertexArray             m_VAO;
  CVertexBuffer            m_VBO;
};
