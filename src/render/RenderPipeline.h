#pragma once

#include "interfaces/RenderPass.h"
#include "render/RenderTypes.h"
#include "render/ShaderTypes.h"
#include "render/Buffer.h"
#include <vector>
#include <memory>

class CRenderQueue;
class IRenderer;
struct TFrameContext;

class CRenderPipeline final
{
public:
  CRenderPipeline();
  ~CRenderPipeline();

  void Init();
  void Render(CRenderQueue &_Queue, IRenderer &_Renderer);

  void SetLightingData(const std::vector<TLight> &_Lighting);

private:
  glm::mat4 CalculateLightSpaceMatrix() const;

  void SortCommands(std::vector<TRenderCommand> &_Commands, const TFrameContext &_FrameContext);
  std::span<TRenderCommand> FilterCommands(const std::unique_ptr<IRenderPass> &_RenderPass, std::vector<TRenderCommand> &_Commands);

  TFrameContext CreateFrameContext(IRenderer &_Renderer);

private:
  std::vector<std::unique_ptr<IRenderPass>> m_RenderPasses;

  TShaderLighting   m_Lighting;
  CUniformBuffer    m_LightingUBO;
  mutable glm::mat4 m_LightSpaceMatrix;
  mutable bool      m_LightSpaceMatrixDirty;
};