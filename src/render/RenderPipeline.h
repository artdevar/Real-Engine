#pragma once

#include "interfaces/RenderPass.h"
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

class CRenderPipeline final
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

  void Init();
  void Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer);

private:
  void BeginFrame(IRenderer &_Renderer);
  void EndFrame(IRenderer &_Renderer);

  void ShadowPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void GeometryPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);
  void PostProcessPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands);

  void DoRenderPass(const std::unique_ptr<IRenderPass> &_RenderPass,
                    IRenderer                          &_Renderer,
                    TRenderContext                     &_RenderContext,
                    std::vector<TRenderCommand>        &_Commands);

  TRenderContext CreateRenderContext(IRenderer &_Renderer);

  void SetLightingData(const std::vector<TLight> &_Lighting);
  glm::mat4 CalculateLightSpaceMatrix() const;

  void SortCommands(std::vector<TRenderCommand> &_Commands, const TRenderContext &_RenderContext);
  std::span<TRenderCommand> FilterCommands(const std::unique_ptr<IRenderPass> &_RenderPass, std::vector<TRenderCommand> &_Commands);

  std::shared_ptr<CTextureBase> CreateRenderTexture(const std::string &_Name, int _MultisampleCount, TVector2i _Size) const;

private:
  std::vector<std::unique_ptr<IRenderPass>> m_ShadowPasses;
  std::vector<std::unique_ptr<IRenderPass>> m_GeometryPasses;
  std::vector<std::unique_ptr<IRenderPass>> m_PostProcessPasses;

  TFBO m_SceneFBO;
  TFBO m_HdrFBO;

  TShaderLighting m_Lighting;
  CUniformBuffer  m_LightingUBO;
};