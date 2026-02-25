#pragma once

#include "interfaces/RenderPass.h"
#include "interfaces/Sharable.h"
#include "interfaces/Shutdownable.h"
#include "interfaces/EventsListener.h"
#include "render/RenderTypes.h"
#include "render/ShaderTypes.h"
#include "render/Buffer.h"
#include "engine/MathCore.h"
#include <vector>
#include <memory>

class CRenderQueue;
class IRenderer;
class CTextureBase;
struct TRenderContext;
struct TFrameData;

class CRenderPipeline final : public CSharable<CRenderPipeline>,
                              public IEventsListener,
                              public IShutdownable
{
  struct TFBO
  {
    CFrameBuffer                  FrameBuffer;
    CRenderBuffer                 RenderBuffer;
    std::shared_ptr<CTextureBase> Texture;
  };

public:
  CRenderPipeline();
  ~CRenderPipeline();

  void Shutdown() override;

  void OnEvent(const TEvent &_Event) override;

  void Init();
  void Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer);

private:
  void BeginFrame(IRenderer &_Renderer);
  void EndFrame(IRenderer &_Renderer);

  void ShadowPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void GeometryPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void PostProcessPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);

  TRenderContext CreateRenderContext(IRenderer &_Renderer);

  void InitFBO(TVector2i _Size);

  void SetLightingData(const std::vector<TLight> &_Lighting);
  glm::mat4 CalculateLightSpaceMatrix() const;

  static void SortCommands(std::vector<TRenderCommand> &_Commands, const TRenderContext &_RenderContext);
  static std::span<TRenderCommand> FilterCommands(const std::shared_ptr<IRenderPass> &_RenderPass, std::vector<TRenderCommand> &_Commands);
  static std::shared_ptr<CTextureBase> CreateRenderTexture(const std::string &_Name, TVector2i _Size);

  static void DoRenderPass(const std::shared_ptr<IRenderPass> &_RenderPass,
                           IRenderer                          &_Renderer,
                           TRenderContext                     &_RenderContext,
                           std::vector<TRenderCommand>        &_Commands);

private:
  static const std::string RENDER_TEXTURE_NAME;

  std::vector<std::shared_ptr<IRenderPass>> m_ShadowPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_GeometryPasses;
  std::vector<std::shared_ptr<IRenderPass>> m_PostProcessPasses;

  std::unique_ptr<TFBO> m_SceneFBO;

  TShaderLighting m_Lighting;
  CUniformBuffer  m_LightingUBO;
};