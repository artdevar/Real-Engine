#pragma once

#include "interfaces/RenderPipeline.h"
#include "render/RenderTypes.h"
#include "render/ShaderTypes.h"
#include "render/Buffer.h"
#include "passes/RenderPassTypes.h"
#include <events/EventsListener.h>
#include <common/Sharable.h>
#include <common/MathTypes.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <span>

class IRenderer;
class IRenderPass;
class CRenderQueue;
class CTextureBase;
struct TRenderContext;
struct TRenderTarget;
struct TFrameData;
struct TRenderCommand;

class CRenderPipeline final : public CSharable<CRenderPipeline>,
                              public IEventsListener,
                              public IRenderPipeline
{
public:
  CRenderPipeline();
  ~CRenderPipeline();

  void Shutdown() override;

  void OnEvent(const TEvent &_Event) override;

  void Init(TVector2i _Viewport) override;
  void Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer) override;

  uint32_t GetDrawCallsCount() const override;
  uint32_t GetVerticesCount() const override;
  uint32_t GetIndicesCount() const override;
  uint32_t GetTrianglesCount() const override;
  uint32_t GetLinesCount() const override;
  uint32_t GetPointsCount() const override;
  uint32_t GetRenderTextureID() const override;

private:
  void BeginFrame(IRenderer &_Renderer);
  void EndFrame(IRenderer &_Renderer);

  void UtilityPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void ShadowPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void GeometryPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void DebugPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void PostProcessPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void OutputPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);

  void SetLightingData(const std::vector<TLight> &_Lighting);
  glm::mat4 CalculateLightSpaceMatrix() const;

  TRenderContext CreateRenderContext(IRenderer &_Renderer);
  std::unique_ptr<TRenderTarget> CreateRenderTarget(const std::string &_Name, TVector2i _Size);

private:
  static std::string GetRenderTextureName();
  static std::shared_ptr<CTextureBase> CreateRenderTexture(const std::string &_Name, TVector2i _Size);
  static std::span<TRenderCommand> FilterCommands(const std::shared_ptr<IRenderPass> &_RenderPass, std::vector<TRenderCommand> &_Commands);
  static void SortCommands(std::vector<TRenderCommand> &_Commands, const TRenderContext &_RenderContext);
  static void DoRenderPass(const std::shared_ptr<IRenderPass> &_RenderPass,
                           IRenderer                          &_Renderer,
                           TRenderContext                     &_RenderContext,
                           std::vector<TRenderCommand>        &_Commands);

  static bool DoesRenderPassExists(ERenderPassType Type, const std::vector<std::shared_ptr<IRenderPass>> &_Container);
  static void RemoveRenderPass(ERenderPassType Type, std::vector<std::shared_ptr<IRenderPass>> &_Container);

private:
  std::vector<std::shared_ptr<IRenderPass>> m_UtilityPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_ShadowPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_GeometryPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_DebugPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_PostProcessPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_OutputPasses;

  std::unique_ptr<TRenderTarget> m_SceneTarget;
  std::unique_ptr<TRenderTarget> m_PostProcessTarget;
  std::unique_ptr<TRenderTarget> m_FinalTarget;

  TShaderLighting m_Lighting;
  CUniformBuffer  m_LightingUBO;

  uint32_t m_LastFrameDrawCalls;
  uint32_t m_LastFrameVertices;
  uint32_t m_LastFrameIndices;
  uint32_t m_LastFrameTriangles;
  uint32_t m_LastFrameLines;
  uint32_t m_LastFramePoints;
};