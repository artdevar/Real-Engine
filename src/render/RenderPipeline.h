#pragma once

#include "interfaces/RenderPipeline.h"
#include "render/ShaderTypes.h"
#include "render/FrameData.h"
#include "passes/RenderPassTypes.h"
#include "render/Buffer.h"
#include <events/EventsListener.h>
#include <common/Sharable.h>
#include <common/MathTypes.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <map>

class IRenderer;
class IRenderPass;
class CRenderQueue;
class CTextureBase;
class CVertexArray;
class CVertexBuffer;
struct TRenderContext;
struct TRenderTarget;
struct TRenderCommand;

class CRenderPipeline final : public CSharable<CRenderPipeline>,
                              public IEventsListener,
                              public IRenderPipeline
{
  struct TBuffers
  {
    CVertexArray  VAO;
    CVertexBuffer VBO = CVertexBuffer(GL_STATIC_DRAW);
  };

  struct TRenderPass
  {
    TRenderPass(std::shared_ptr<IRenderPass> _Pass, bool _IsEnabled) :
        Pass(std::move(_Pass)),
        IsEnabled(_IsEnabled)
    {
    }

    std::shared_ptr<IRenderPass> Pass;
    bool                         IsEnabled;
  };

  using TRenderPassesList = std::vector<TRenderPass>;

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
  uint32_t GetShadowMapTextureID() const override;

  float GetRenderPassTime(ERenderPassType _Type) const override;

private:
  void BeginFrame(IRenderer &_Renderer, const TRenderContext &_RenderContext);
  void EndFrame(IRenderer &_Renderer, const TRenderContext &_RenderContext);

  void UtilityPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void ShadowPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void GeometryPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void DebugPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void PostProcessPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void OutputPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);

  void SetLightingData(const std::vector<TFrameData::TLight> &_Lighting);
  glm::mat4 CalculateLightSpaceMatrix() const;

  TRenderContext CreateRenderContext(const TFrameData &FrameData, IRenderer &_Renderer);
  void InitRenderTargets(TVector2i _Viewport);
  void InitCommonVAOs();

  void DoRenderPass(const std::shared_ptr<IRenderPass> &_RenderPass,
                    IRenderer                          &_Renderer,
                    TRenderContext                     &_RenderContext,
                    const std::vector<TRenderCommand>  &_Commands);

private:
  static std::shared_ptr<CTextureBase> CreateRenderTexture(const std::string &_Name, TVector2i _Size);
  static std::shared_ptr<CTextureBase> CreateDepthTexture(const std::string &_Name, TVector2i _Size);
  static std::shared_ptr<CTextureBase> CreateVelocityTexture(const std::string &_Name, TVector2i _Size);
  static std::vector<const TRenderCommand *> FilterCommands(const std::shared_ptr<IRenderPass> &_RenderPass,
                                                            const std::vector<TRenderCommand>  &_Commands);
  static void SortCommands(std::vector<TRenderCommand> &_Commands, const TRenderContext &_RenderContext);
  static bool IsRenderPassEnabled(ERenderPassType _Type, const TRenderPassesList &_Passes);
  static void SetRenderPassEnabled(ERenderPassType _Type, bool _Enabled, TRenderPassesList &_Passes);
  static glm::vec2 GenerateHaltonJitter(uint32_t _Index, int32_t _Samples);

private:
  TRenderPassesList m_UtilityPasses;
  TRenderPassesList m_ShadowPasses;
  TRenderPassesList m_GeometryPasses;
  TRenderPassesList m_DebugPasses;
  TRenderPassesList m_PostProcessPasses;
  TRenderPassesList m_OutputPasses;

  std::unique_ptr<TRenderTarget> m_SceneTarget;
  std::unique_ptr<TRenderTarget> m_PostProcessTarget;
  std::unique_ptr<TRenderTarget> m_FinalTarget;

  TBuffers m_QuadBuffer;
  TBuffers m_CubeBuffer;

  TShaderLighting m_Lighting;
  CUniformBuffer  m_LightingUBO;

  uint32_t m_LastFrameDrawCalls;
  uint32_t m_LastFrameVertices;
  uint32_t m_LastFrameIndices;
  uint32_t m_LastFrameTriangles;
  uint32_t m_LastFrameLines;
  uint32_t m_LastFramePoints;
  uint32_t m_ShadowMapTextureID;

  std::map<ERenderPassType, float> m_RenderPassTimes;

  glm::mat4 m_PrevViewProjectionMatrix;
  glm::mat4 m_PrevJitteredViewProjectionMatrix;
  glm::vec2 m_CurrentJitter;
  glm::vec2 m_PreviousJitter;
  uint32_t  m_JitterFrameIndex;
  int32_t   m_JitterSampleCount;
  bool      m_IsTAAEnabled;
};
