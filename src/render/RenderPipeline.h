#pragma once

#include "interfaces/RenderPass.h"
#include "render/RenderTypes.h"
#include "render/ShaderTypes.h"
#include "render/Buffer.h"
#include <vector>
#include <memory>

class CRenderQueue;
class IRenderer;
struct TRenderContext;
struct TFrameData;

class CRenderPipeline final
{
public:
  CRenderPipeline();
  ~CRenderPipeline();

  void Init();
  void Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer);

private:
  void BeginFrame(IRenderer &_Renderer);
  void EndFrame(IRenderer &_Renderer);

  TRenderContext CreateRenderContext(IRenderer &_Renderer);

  void SetLightingData(const std::vector<TLight> &_Lighting);
  glm::mat4 CalculateLightSpaceMatrix() const;

  void SortCommands(std::vector<TRenderCommand> &_Commands, const TRenderContext &_RenderContext);
  std::span<TRenderCommand> FilterCommands(const std::unique_ptr<IRenderPass> &_RenderPass, std::vector<TRenderCommand> &_Commands);

private:
  std::vector<std::unique_ptr<IRenderPass>> m_RenderPasses;

  TShaderLighting m_Lighting;
  CUniformBuffer  m_LightingUBO;
};