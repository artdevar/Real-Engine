#include "pch.h"

#include "RenderPipeline.h"
#include "RenderQueue.h"
#include "RenderTarget.h"
#include "FrameData.h"
#include "passes/OpaqueRenderPass.h"
#include "passes/SkyboxRenderPass.h"
#include "passes/TransparentRenderPass.h"
#include "passes/ShadowRenderPass.h"
#include "passes/PostProcessRenderPass.h"
#include "passes/OutputRenderPass.h"
#include "passes/CollisionRenderPass.h"
#include "passes/GridRenderPass.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Camera.h"
#include "engine/Config.h"
#include "engine/Engine.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "utils/Resource.h"
#include <common/Logger.h>
#include <glm/gtx/norm.hpp>

CRenderPipeline::CRenderPipeline() :
    m_Lighting({}),
    m_LightingUBO(GL_DYNAMIC_DRAW),
    m_LastFrameDrawCalls(0),
    m_LastFrameVertices(0),
    m_LastFrameIndices(0),
    m_LastFrameTriangles(0),
    m_LastFrameLines(0),
    m_LastFramePoints(0)
{
}

CRenderPipeline::~CRenderPipeline() = default;

void CRenderPipeline::Shutdown()
{
  m_SceneTarget.reset();
  m_PostProcessTarget.reset();
}

void CRenderPipeline::Init(TVector2i _Viewport)
{
  const bool EditorEnabled    = CConfig::Instance().IsEditorEnabled();
  const bool ShadowsEnabled   = CConfig::Instance().GetShadowsEnabled();
  const bool GridEnabled      = CConfig::Instance().GetGridEnabled();
  const bool WireframeEnabled = CConfig::Instance().GetWireframeEnabled();

  if (ShadowsEnabled)
    m_ShadowPasses.push_back(CShadowRenderPass::Create());

  if (EditorEnabled)
  {
    if (WireframeEnabled)
      m_DebugPasses.push_back(CCollisionRenderPass::Create());

    if (GridEnabled)
      m_DebugPasses.push_back(CGridRenderPass::Create());
  }

  m_GeometryPasses.push_back(COpaqueRenderPass::Create());
  m_GeometryPasses.push_back(CSkyboxRenderPass::Create());
  m_GeometryPasses.push_back(CTransparentRenderPass::Create());
  m_PostProcessPasses.push_back(CPostProcessRenderPass::Create());
  m_OutputPasses.push_back(COutputRenderPass::Create());

  m_SceneTarget       = CreateRenderTarget(GetRenderTextureName(), _Viewport);
  m_PostProcessTarget = CreateRenderTarget(GetRenderTextureName(), _Viewport);
  if (EditorEnabled)
    m_FinalTarget = CreateRenderTarget(GetRenderTextureName(), _Viewport);

  m_LightingUBO.Bind();
  m_LightingUBO.Reserve(sizeof(TShaderLighting));
  m_LightingUBO.BindToBase(BINDING_LIGHTING_BUFFER);
  m_LightingUBO.Unbind();

  event::Subscribe(TEventType::ViewportResized, GetWeakPtr());
  event::Subscribe(TEventType::Config_ShadowsEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_GridEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_WireframeEnabledChanged, GetWeakPtr());
}

void CRenderPipeline::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::Config_ShadowsEnabledChanged: {
    const bool ShadowsEnabled       = DoesRenderPassExists(ERenderPassType::Shadow, m_ShadowPasses);
    const bool NeedToEnableShadows  = _Event.GetValue<bool>() && !ShadowsEnabled;
    const bool NeedToDisableShadows = !_Event.GetValue<bool>() && ShadowsEnabled;

    if (NeedToEnableShadows)
      m_ShadowPasses.push_back(CShadowRenderPass::Create());
    else if (NeedToDisableShadows)
      RemoveRenderPass(ERenderPassType::Shadow, m_ShadowPasses);

    break;
  }
  case TEventType::Config_GridEnabledChanged: {
    const bool GridEnabled       = DoesRenderPassExists(ERenderPassType::Grid, m_DebugPasses);
    const bool NeedToEnableGrid  = _Event.GetValue<bool>() && !GridEnabled;
    const bool NeedToDisableGrid = !_Event.GetValue<bool>() && GridEnabled;

    if (NeedToEnableGrid)
      m_DebugPasses.push_back(CGridRenderPass::Create());
    else if (NeedToDisableGrid)
      RemoveRenderPass(ERenderPassType::Grid, m_DebugPasses);

    break;
  }
  case TEventType::Config_WireframeEnabledChanged: {
    const bool WireframeEnabled       = DoesRenderPassExists(ERenderPassType::Collision, m_DebugPasses);
    const bool NeedToEnableWireframe  = _Event.GetValue<bool>() && !WireframeEnabled;
    const bool NeedToDisableWireframe = !_Event.GetValue<bool>() && WireframeEnabled;

    if (NeedToEnableWireframe)
      m_DebugPasses.push_back(CCollisionRenderPass::Create());
    else if (NeedToDisableWireframe)
      RemoveRenderPass(ERenderPassType::Collision, m_DebugPasses);

    break;
  }
  case TEventType::ViewportResized: {
    const TVector2i NewViewport = _Event.GetValue<TVector2i>();

    m_SceneTarget       = CreateRenderTarget(GetRenderTextureName(), NewViewport);
    m_PostProcessTarget = CreateRenderTarget(GetRenderTextureName(), NewViewport);
    if (m_FinalTarget)
      m_FinalTarget = CreateRenderTarget(GetRenderTextureName(), NewViewport);

    break;
  }
  }
}

void CRenderPipeline::Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer)
{
  BeginFrame(_Renderer);

  SetLightingData(FrameData.Lights);

  std::vector<TRenderCommand> Commands      = _Queue.StealCommands();
  TRenderContext              RenderContext = CreateRenderContext(_Renderer);

  SortCommands(Commands, RenderContext);

  ShadowPass(_Renderer, RenderContext, Commands);
  GeometryPass(_Renderer, RenderContext, Commands);
  PostProcessPass(_Renderer, RenderContext, Commands);

  DebugPass(_Renderer, RenderContext, Commands);
  OutputPass(_Renderer, RenderContext, Commands);

  EndFrame(_Renderer);
}

void CRenderPipeline::BeginFrame(IRenderer &_Renderer)
{
  _Renderer.OnFrameBegin();
  _Renderer.SetViewport(CEngine::Instance().GetViewportSize());
  _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));
  _Renderer.ClearColor({0.2f, 0.2f, 0.2f, 1.0f});
}

void CRenderPipeline::EndFrame(IRenderer &_Renderer)
{
  m_LastFrameDrawCalls = _Renderer.GetDrawCallsCount();
  m_LastFrameVertices  = _Renderer.GetVerticesCount();
  m_LastFrameIndices   = _Renderer.GetIndicesCount();
  m_LastFrameTriangles = _Renderer.GetTrianglesCount();
  m_LastFrameLines     = _Renderer.GetLinesCount();
  m_LastFramePoints    = _Renderer.GetPointsCount();

  _Renderer.CheckErrors();
}

void CRenderPipeline::ShadowPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  _RenderContext.SceneRenderTarget.FrameBuffer.Unbind();

  for (const std::shared_ptr<IRenderPass> &RenderPass : m_ShadowPasses)
    DoRenderPass(RenderPass, _Renderer, _RenderContext, _Commands);
}

void CRenderPipeline::GeometryPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  _RenderContext.SceneRenderTarget.FrameBuffer.Bind();

  _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));

  for (const std::shared_ptr<IRenderPass> &RenderPass : m_GeometryPasses)
    DoRenderPass(RenderPass, _Renderer, _RenderContext, _Commands);

  _RenderContext.SceneRenderTarget.FrameBuffer.Unbind();
}

void CRenderPipeline::PostProcessPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  _RenderContext.PostProcessRenderTarget.FrameBuffer.Bind();

  for (const std::shared_ptr<IRenderPass> &RenderPass : m_PostProcessPasses)
    DoRenderPass(RenderPass, _Renderer, _RenderContext, _Commands);

  _RenderContext.PostProcessRenderTarget.FrameBuffer.Unbind();
}

void CRenderPipeline::DebugPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  if (m_DebugPasses.empty())
    return;

  CFrameBuffer::Blit(_RenderContext.SceneRenderTarget.FrameBuffer.ID(),       //
                     _RenderContext.PostProcessRenderTarget.FrameBuffer.ID(), //
                     _RenderContext.SceneRenderTarget.Size.X,                 //
                     _RenderContext.SceneRenderTarget.Size.Y,                 //
                     GL_DEPTH_BUFFER_BIT,                                     //
                     GL_NEAREST);

  _RenderContext.PostProcessRenderTarget.FrameBuffer.Bind();

  for (const std::shared_ptr<IRenderPass> &RenderPass : m_DebugPasses)
    DoRenderPass(RenderPass, _Renderer, _RenderContext, _Commands);

  _RenderContext.PostProcessRenderTarget.FrameBuffer.Unbind();
}

void CRenderPipeline::OutputPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  if (_RenderContext.FinalRenderTarget)
    _RenderContext.FinalRenderTarget->FrameBuffer.Bind();
  else
    _RenderContext.PostProcessRenderTarget.FrameBuffer.Unbind();

  for (const std::shared_ptr<IRenderPass> &RenderPass : m_OutputPasses)
    DoRenderPass(RenderPass, _Renderer, _RenderContext, _Commands);

  if (_RenderContext.FinalRenderTarget)
    _RenderContext.FinalRenderTarget->FrameBuffer.Unbind();
}

void CRenderPipeline::DoRenderPass(const std::shared_ptr<IRenderPass> &_RenderPass,
                                   IRenderer                          &_Renderer,
                                   TRenderContext                     &_RenderContext,
                                   std::vector<TRenderCommand>        &_Commands)
{

  if (!_RenderPass->IsAvailable())
    return;

  std::span<TRenderCommand> CommandsSpan = FilterCommands(_RenderPass, _Commands);

  _RenderPass->PreExecute(_Renderer, _RenderContext, CommandsSpan);
  _RenderPass->Execute(_Renderer, _RenderContext, CommandsSpan);
  _RenderPass->PostExecute(_Renderer, _RenderContext, CommandsSpan);
}

void CRenderPipeline::SortCommands(std::vector<TRenderCommand> &_Commands, const TRenderContext &_RenderContext)
{
  auto It = std::stable_partition(_Commands.begin(), _Commands.end(), [](const TRenderCommand &Command) {
    return Command.Material.AlphaMode != EAlphaMode::Blend;
  });

  const glm::vec3 CamPos = _RenderContext.CameraPosition;

  std::stable_sort(It, _Commands.end(), [&CamPos](const TRenderCommand &A, const TRenderCommand &B) {
    const glm::vec3 PosA = glm::vec3(A.ModelMatrix[3]);
    const glm::vec3 PosB = glm::vec3(B.ModelMatrix[3]);

    const float DistA = glm::length2(CamPos - PosA);
    const float DistB = glm::length2(CamPos - PosB);

    return DistA > DistB;
  });
}

std::span<TRenderCommand> CRenderPipeline::FilterCommands(const std::shared_ptr<IRenderPass> &_RenderPass, std::vector<TRenderCommand> &_Commands)
{
  auto It = std::stable_partition(_Commands.begin(), _Commands.end(), [&_RenderPass](const TRenderCommand &Command) {
    return _RenderPass->Accepts(Command);
  });

  return std::span<TRenderCommand>(_Commands.begin(), It);
}

void CRenderPipeline::SetLightingData(const std::vector<TLight> &_Lighting)
{
  TShaderLighting ShaderLighting;
  std::memset(&ShaderLighting, 0, sizeof(TShaderLighting));

  for (const TLight &Light : _Lighting)
  {
    switch (Light.Type)
    {
    case ELightType::Directional:
      ShaderLighting.LightDirectional = TLightDirectional{
          .Direction = Light.Direction,
          .Color     = Light.Color,
          .Intensity = Light.Intensity,
      };
      break;
    }
  }

  m_Lighting = std::move(ShaderLighting);
  m_LightingUBO.Bind();
  m_LightingUBO.Assign(&m_Lighting, sizeof(TShaderLighting));
  m_LightingUBO.Unbind();
}

glm::mat4 CRenderPipeline::CalculateLightSpaceMatrix() const
{
  const float NearPlane = CConfig::Instance().GetLightSpaceMatrixZNear();
  const float FarPlane  = CConfig::Instance().GetLightSpaceMatrixZFar();
  const float LeftBot   = CConfig::Instance().GetLightSpaceMatrixOrthLeftBot();
  const float RightTop  = CConfig::Instance().GetLightSpaceMatrixOrthRightTop();

  const glm::vec3 LightDir        = m_Lighting.LightDirectional.Direction * -1.0f;
  const glm::mat4 LightProjection = glm::ortho(LeftBot, RightTop, LeftBot, RightTop, NearPlane, FarPlane);
  const glm::mat4 LightView       = glm::lookAt(LightDir, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

  return LightProjection * LightView;
}

TRenderContext CRenderPipeline::CreateRenderContext(IRenderer &_Renderer)
{
  const auto &Camera = _Renderer.GetCamera();

  return TRenderContext{
      .SceneRenderTarget       = *m_SceneTarget,
      .PostProcessRenderTarget = *m_PostProcessTarget,
      .FinalRenderTarget       = m_FinalTarget.get(),
      .CameraPosition          = Camera->GetPosition(),
      .ProjectionMatrix        = Camera->GetProjection(),
      .ViewMatrix              = Camera->GetView(),
      .ViewProjectionMatrix    = Camera->GetProjection() * Camera->GetView(),
      .LightSpaceMatrix        = CalculateLightSpaceMatrix(),
      .ShadowMap               = 0,
  };
}

std::unique_ptr<TRenderTarget> CRenderPipeline::CreateRenderTarget(const std::string &_Name, TVector2i _Size)
{
  auto RenderTarget = std::make_unique<TRenderTarget>();

  RenderTarget->Size  = _Size;
  RenderTarget->Color = CreateRenderTexture(_Name, _Size);

  RenderTarget->Depth.Bind();
  RenderTarget->Depth.AllocateStorage(GL_DEPTH_COMPONENT, _Size.X, _Size.Y);
  RenderTarget->Depth.Unbind();

  RenderTarget->FrameBuffer.Bind();
  RenderTarget->FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, RenderTarget->Color->ID());
  RenderTarget->FrameBuffer.AttachRenderbuffer(GL_DEPTH_ATTACHMENT, RenderTarget->Depth.ID());
  RenderTarget->FrameBuffer.Unbind();

  return RenderTarget;
}

std::shared_ptr<CTextureBase> CRenderPipeline::CreateRenderTexture(const std::string &_Name, TVector2i _Size)
{
  TTextureParams TextureParams;
  TextureParams.Width          = _Size.X;
  TextureParams.Height         = _Size.Y;
  TextureParams.InternalFormat = GL_RGBA16F;
  TextureParams.Format         = GL_RGBA;
  TextureParams.Type           = GL_FLOAT;
  TextureParams.MinFilter      = ETextureFilter::Linear;
  TextureParams.MagFilter      = ETextureFilter::Linear;

  return resource::RecreateTexture(_Name, TextureParams);
}

std::string CRenderPipeline::GetRenderTextureName()
{
  constexpr std::string_view RENDER_TEXTURE_NAME = "PIPELINE_SCENE_RENDER_TEXTURE_";

  static uint32_t Counter = 0;
  return std::format("{}{}", RENDER_TEXTURE_NAME, Counter++);
}

uint32_t CRenderPipeline::GetRenderTextureID() const
{
  return m_FinalTarget ? m_FinalTarget->Color->ID() : 0;
}

uint32_t CRenderPipeline::GetDrawCallsCount() const
{
  return m_LastFrameDrawCalls;
}

uint32_t CRenderPipeline::GetVerticesCount() const
{
  return m_LastFrameVertices;
}

uint32_t CRenderPipeline::GetIndicesCount() const
{
  return m_LastFrameIndices;
}

uint32_t CRenderPipeline::GetTrianglesCount() const
{
  return m_LastFrameTriangles;
}

uint32_t CRenderPipeline::GetLinesCount() const
{
  return m_LastFrameLines;
}

uint32_t CRenderPipeline::GetPointsCount() const
{
  return m_LastFramePoints;
}

bool CRenderPipeline::DoesRenderPassExists(ERenderPassType Type, const std::vector<std::shared_ptr<IRenderPass>> &_Container)
{
  return std::any_of(_Container.begin(), _Container.end(), [Type](const std::shared_ptr<IRenderPass> &RenderPass) {
    return RenderPass->GetType() == Type;
  });
}

void CRenderPipeline::RemoveRenderPass(ERenderPassType Type, std::vector<std::shared_ptr<IRenderPass>> &_Container)
{
  auto It = std::remove_if(_Container.begin(), _Container.end(), [Type](const std::shared_ptr<IRenderPass> &RenderPass) {
    return RenderPass->GetType() == Type;
  });

  if (It != _Container.end())
    _Container.erase(It, _Container.end());
}