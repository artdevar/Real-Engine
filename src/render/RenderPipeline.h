#pragma once

#include "interfaces/RenderPass.h"
#include "interfaces/Sharable.h"
#include "interfaces/Shutdownable.h"
#include "interfaces/EventsListener.h"
#include "render/RenderTypes.h"
#include "render/ShaderTypes.h"
#include "render/Buffer.h"
#include <common/MathTypes.h>
#include <cstdint>
#include <vector>
#include <memory>

class CRenderQueue;
class IRenderer;
class CTextureBase;
struct TRenderContext;
struct TRenderTarget;
struct TFrameData;

class CRenderPipeline final : public CSharable<CRenderPipeline>,
                              public IEventsListener,
                              public IShutdownable
{
public:
  CRenderPipeline();
  ~CRenderPipeline();

  void Shutdown() override;

  void OnEvent(const TEvent &_Event) override;

  void Init();
  void Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer);
  uint32_t GetRenderTextureID() const;

private:
  void BeginFrame(IRenderer &_Renderer);
  void EndFrame(IRenderer &_Renderer);

  void ShadowPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void GeometryPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void PostProcessPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void OutputPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);

  void SetLightingData(const std::vector<TLight> &_Lighting);
  glm::mat4 CalculateLightSpaceMatrix() const;

  TRenderContext CreateRenderContext(IRenderer &_Renderer);
  std::unique_ptr<TRenderTarget> CreateRenderTarget(const std::string &_Name, TVector2i _Size);

private:
  static std::shared_ptr<CTextureBase> CreateRenderTexture(const std::string &_Name, TVector2i _Size);
  static std::span<TRenderCommand> FilterCommands(const std::shared_ptr<IRenderPass> &_RenderPass, std::vector<TRenderCommand> &_Commands);
  static void SortCommands(std::vector<TRenderCommand> &_Commands, const TRenderContext &_RenderContext);
  static void DoRenderPass(const std::shared_ptr<IRenderPass> &_RenderPass,
                           IRenderer                          &_Renderer,
                           TRenderContext                     &_RenderContext,
                           std::vector<TRenderCommand>        &_Commands);

private:
  static const std::string RENDER_TEXTURE_NAME;

  std::vector<std::shared_ptr<IRenderPass>> m_ShadowPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_GeometryPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_PostProcessPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_OutputPasses;

  std::unique_ptr<TRenderTarget> m_SceneTarget;
  std::unique_ptr<TRenderTarget> m_PostProcessTarget;

  TShaderLighting m_Lighting;
  CUniformBuffer  m_LightingUBO;
};