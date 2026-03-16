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
#include "passes/BloomRenderPass.h"
#include "passes/OutputRenderPass.h"
#include "passes/CollisionRenderPass.h"
#include "passes/GridRenderPass.h"
#include "passes/EquirectangularToCubemapPass.h"
#include "passes/IrradianceConvolutionPass.h"
#include "passes/TAARenderPass.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Camera.h"
#include "engine/Config.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "utils/Resource.h"
#include <common/Logger.h>
#include <common/Stopwatch.h>
#include <common/Clock.h>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

CRenderPipeline::CRenderPipeline() :
    m_Lighting({}),
    m_LightingUBO(GL_DYNAMIC_DRAW),
    m_LastFrameDrawCalls(0),
    m_LastFrameVertices(0),
    m_LastFrameIndices(0),
    m_LastFrameTriangles(0),
    m_LastFrameLines(0),
    m_LastFramePoints(0),
    m_PrevViewProjectionMatrix(glm::mat4(1.0f)),
    m_CurrentJitter(0.0f),
    m_PreviousJitter(0.0f),
    m_JitterFrameIndex(0),
    m_JitterSampleCount(8),
    m_IsTAAEnabled(false)
{
}

CRenderPipeline::~CRenderPipeline() = default;

void CRenderPipeline::Shutdown()
{
  m_SceneTarget.reset();
  m_PostProcessTarget.reset();
  m_FinalTarget.reset();
}

void CRenderPipeline::Init(TVector2i _Viewport)
{
  const bool EditorEnabled    = CConfig::Instance().IsEditorEnabled();
  const bool ShadowsEnabled   = CConfig::Instance().GetShadowsEnabled();
  const bool GridEnabled      = CConfig::Instance().GetGridEnabled();
  const bool WireframeEnabled = CConfig::Instance().GetWireframeEnabled();
  const bool BloomEnabled     = CConfig::Instance().GetBloomEnabled();

  m_IsTAAEnabled      = CConfig::Instance().GetTAAEnabled();
  m_JitterSampleCount = CConfig::Instance().GetTAAJitterSampleCount();

  m_ShadowPasses.emplace_back(CShadowRenderPass::Create(), ShadowsEnabled);

  m_UtilityPasses.emplace_back(CEquirectangularToCubemapPass::Create(), true);
  m_UtilityPasses.emplace_back(CIrradianceConvolutionPass::Create(), true);

  m_GeometryPasses.emplace_back(COpaqueRenderPass::Create(), true);
  m_GeometryPasses.emplace_back(CSkyboxRenderPass::Create(), true);
  m_GeometryPasses.emplace_back(CTransparentRenderPass::Create(), true);

  m_PostProcessPasses.emplace_back(CTAARenderPass::Create(_Viewport), m_IsTAAEnabled);
  m_PostProcessPasses.emplace_back(CBloomRenderPass::Create(_Viewport), BloomEnabled);
  m_PostProcessPasses.emplace_back(CPostProcessRenderPass::Create(), true);

  m_DebugPasses.emplace_back(CCollisionRenderPass::Create(), WireframeEnabled);
  m_DebugPasses.emplace_back(CGridRenderPass::Create(), GridEnabled);

  m_OutputPasses.emplace_back(COutputRenderPass::Create(), true);

  m_LightingUBO.Bind();
  m_LightingUBO.Reserve(sizeof(TShaderLighting));
  m_LightingUBO.BindToBase(BINDING_LIGHTING_BUFFER);
  m_LightingUBO.Unbind();

  InitRenderTargets(_Viewport);
  InitCommonVAOs();

  event::Subscribe(TEventType::ViewportResized, GetWeakPtr());
  event::Subscribe(TEventType::Config_ShadowsEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_GridEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_WireframeEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_BloomEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_TAAEnabledChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_TAAJitterSampleCountChanged, GetWeakPtr());
}

void CRenderPipeline::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::ViewportResized: {
    const TVector2i NewViewport = _Event.GetValue<TVector2i>();
    InitRenderTargets(NewViewport);
    break;
  }
  case TEventType::Config_ShadowsEnabledChanged: {
    SetRenderPassEnabled(ERenderPassType::Shadow, _Event.GetValue<bool>(), m_ShadowPasses);
    break;
  }
  case TEventType::Config_GridEnabledChanged: {
    SetRenderPassEnabled(ERenderPassType::Grid, _Event.GetValue<bool>(), m_DebugPasses);
    break;
  }
  case TEventType::Config_WireframeEnabledChanged: {
    SetRenderPassEnabled(ERenderPassType::Collision, _Event.GetValue<bool>(), m_DebugPasses);
    break;
  }
  case TEventType::Config_BloomEnabledChanged: {
    SetRenderPassEnabled(ERenderPassType::Bloom, _Event.GetValue<bool>(), m_PostProcessPasses);
    break;
  }
  case TEventType::Config_TAAEnabledChanged: {
    const bool TAAEnabled       = IsRenderPassEnabled(ERenderPassType::TAA, m_PostProcessPasses);
    const bool NeedToEnableTAA  = _Event.GetValue<bool>() && !TAAEnabled;
    const bool NeedToDisableTAA = !_Event.GetValue<bool>() && TAAEnabled;

    if (NeedToEnableTAA)
    {
      const TVector2i Viewport = m_SceneTarget->Size;
      m_SceneTarget->Velocity  = CreateVelocityTexture("SCENE_TARGET_VELOCITY", Viewport);
      m_SceneTarget->FrameBuffer.Bind();
      m_SceneTarget->FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT1, m_SceneTarget->Velocity->ID());
      m_SceneTarget->FrameBuffer.Unbind();
      SetRenderPassEnabled(ERenderPassType::TAA, true, m_PostProcessPasses);
      m_IsTAAEnabled = true;
    }
    else if (NeedToDisableTAA)
    {
      m_SceneTarget->Velocity.reset();
      m_SceneTarget->FrameBuffer.Bind();
      m_SceneTarget->FrameBuffer.DetachTexture(GL_COLOR_ATTACHMENT1);
      m_SceneTarget->FrameBuffer.Unbind();
      SetRenderPassEnabled(ERenderPassType::TAA, false, m_PostProcessPasses);
      m_IsTAAEnabled = false;
    }

    break;
  }
  case TEventType::Config_TAAJitterSampleCountChanged: {
    m_JitterSampleCount = _Event.GetValue<int32_t>();
    break;
  }
  }
}

void CRenderPipeline::Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer)
{
  std::vector<TRenderCommand> Commands      = _Queue.StealCommands();
  TRenderContext              RenderContext = CreateRenderContext(FrameData, _Renderer);

  BeginFrame(_Renderer, RenderContext);
  SortCommands(Commands, RenderContext);
  SetLightingData(FrameData.Lights);

  UtilityPass(_Renderer, RenderContext, Commands);

  ShadowPass(_Renderer, RenderContext, Commands);
  GeometryPass(_Renderer, RenderContext, Commands);
  PostProcessPass(_Renderer, RenderContext, Commands);

  DebugPass(_Renderer, RenderContext, Commands);
  OutputPass(_Renderer, RenderContext, Commands);

  EndFrame(_Renderer, RenderContext);
}

void CRenderPipeline::BeginFrame(IRenderer &_Renderer, const TRenderContext &_RenderContext)
{
  _Renderer.OnFrameBegin();

  _RenderContext.SceneRenderTarget.FrameBuffer.Bind();
  _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));

  if (_RenderContext.FinalRenderTarget)
    _RenderContext.FinalRenderTarget->FrameBuffer.Bind();
  else
    CFrameBuffer::BindDefault();

  _Renderer.ClearColor({0.2f, 0.2f, 0.2f, 1.0f});
}

void CRenderPipeline::EndFrame(IRenderer &_Renderer, const TRenderContext &_RenderContext)
{
  m_PrevViewProjectionMatrix = _RenderContext.ViewProjectionMatrix;
  m_PreviousJitter           = m_CurrentJitter;
  m_JitterFrameIndex         = (m_JitterFrameIndex + 1) % m_JitterSampleCount;

  m_LastFrameDrawCalls = _Renderer.GetDrawCallsCount();
  m_LastFrameVertices  = _Renderer.GetVerticesCount();
  m_LastFrameIndices   = _Renderer.GetIndicesCount();
  m_LastFrameTriangles = _Renderer.GetTrianglesCount();
  m_LastFrameLines     = _Renderer.GetLinesCount();
  m_LastFramePoints    = _Renderer.GetPointsCount();

  _Renderer.CheckErrors();
}

void CRenderPipeline::UtilityPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  for (const TRenderPass &RenderPass : m_UtilityPasses)
  {
    if (RenderPass.IsEnabled)
      DoRenderPass(RenderPass.Pass, _Renderer, _RenderContext, _Commands);
  }
}

void CRenderPipeline::ShadowPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  for (const TRenderPass &RenderPass : m_ShadowPasses)
  {
    if (RenderPass.IsEnabled)
      DoRenderPass(RenderPass.Pass, _Renderer, _RenderContext, _Commands);
  }
}

void CRenderPipeline::GeometryPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  _RenderContext.SceneRenderTarget.FrameBuffer.Bind();
  if (_RenderContext.SceneRenderTarget.Velocity)
  {
    constexpr GLenum Attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    _RenderContext.SceneRenderTarget.FrameBuffer.SetDrawBuffers(Attachments, 2);
  }
  else
  {
    constexpr GLenum Attachments[1] = {GL_COLOR_ATTACHMENT0};
    _RenderContext.SceneRenderTarget.FrameBuffer.SetDrawBuffers(Attachments, 1);
  }

  for (const TRenderPass &RenderPass : m_GeometryPasses)
  {
    if (RenderPass.IsEnabled)
      DoRenderPass(RenderPass.Pass, _Renderer, _RenderContext, _Commands);
  }

  _RenderContext.SceneRenderTarget.FrameBuffer.Unbind();
}

void CRenderPipeline::PostProcessPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  for (const TRenderPass &RenderPass : m_PostProcessPasses)
  {
    if (RenderPass.IsEnabled)
      DoRenderPass(RenderPass.Pass, _Renderer, _RenderContext, _Commands);
  }
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

  for (const TRenderPass &RenderPass : m_DebugPasses)
  {
    if (RenderPass.IsEnabled)
      DoRenderPass(RenderPass.Pass, _Renderer, _RenderContext, _Commands);
  }

  _RenderContext.PostProcessRenderTarget.FrameBuffer.Unbind();
}

void CRenderPipeline::OutputPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  if (_RenderContext.FinalRenderTarget)
    _RenderContext.FinalRenderTarget->FrameBuffer.Bind();
  else
    CFrameBuffer::BindDefault();

  for (const TRenderPass &RenderPass : m_OutputPasses)
  {
    if (RenderPass.IsEnabled)
      DoRenderPass(RenderPass.Pass, _Renderer, _RenderContext, _Commands);
  }

  if (_RenderContext.FinalRenderTarget)
    _RenderContext.FinalRenderTarget->FrameBuffer.Unbind();
}

void CRenderPipeline::DoRenderPass(const std::shared_ptr<IRenderPass> &_RenderPass,
                                   IRenderer                          &_Renderer,
                                   TRenderContext                     &_RenderContext,
                                   const std::vector<TRenderCommand>  &_Commands)
{
  if (!_RenderPass->IsAvailable())
  {
    m_RenderPassTimes[_RenderPass->GetType()] = 0.0f;
    return;
  }

  std::vector<const TRenderCommand *> Commands;
  if (_RenderPass->NeedsCommands())
  {
    Commands = FilterCommands(_RenderPass, _Commands);
    if (Commands.empty())
    {
      m_RenderPassTimes[_RenderPass->GetType()] = 0.0f;
      return;
    }
  }

  utils::CClock Clock;

  _RenderPass->PreExecute(_Renderer, _RenderContext, Commands);
  _RenderPass->Execute(_Renderer, _RenderContext, Commands);
  _RenderPass->PostExecute(_Renderer, _RenderContext, Commands);

  m_RenderPassTimes[_RenderPass->GetType()] = Clock.GetElapsedTimeMs();
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

std::vector<const TRenderCommand *> CRenderPipeline::FilterCommands(const std::shared_ptr<IRenderPass> &_RenderPass,
                                                                    const std::vector<TRenderCommand>  &_Commands)
{
  std::vector<const TRenderCommand *> FilteredCommands;

  for (const TRenderCommand &Command : _Commands)
  {
    if (_RenderPass->Accepts(Command))
      FilteredCommands.push_back(&Command);
  }

  return FilteredCommands;
}

bool CRenderPipeline::IsRenderPassEnabled(ERenderPassType _Type, const TRenderPassesList &_Passes)
{
  auto It = std::find_if(_Passes.begin(), _Passes.end(), [_Type](const TRenderPass &Pass) {
    return Pass.Pass->GetType() == _Type;
  });

  return It != _Passes.end() && It->IsEnabled;
}

void CRenderPipeline::SetRenderPassEnabled(ERenderPassType _Type, bool _Enabled, TRenderPassesList &_Passes)
{
  auto It = std::find_if(_Passes.begin(), _Passes.end(), [_Type](const TRenderPass &Pass) {
    return Pass.Pass->GetType() == _Type;
  });

  if (It != _Passes.end())
    It->IsEnabled = _Enabled;
  else
    assert(false);
}

void CRenderPipeline::SetLightingData(const std::vector<TFrameData::TLight> &_Lighting)
{
  TShaderLighting ShaderLighting;
  std::memset(&ShaderLighting, 0, sizeof(TShaderLighting));

  for (const auto &Light : _Lighting)
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
TRenderContext CRenderPipeline::CreateRenderContext(const TFrameData &FrameData, IRenderer &_Renderer)
{
  const TVector2i Viewport = _Renderer.GetViewport();

  const auto     &Camera     = _Renderer.GetCamera();
  const glm::mat4 Projection = Camera->GetPerspectiveProjection();
  const glm::mat4 View       = Camera->GetView();

  glm::mat4 JitteredProjection = Projection;
  if (m_IsTAAEnabled)
  {
    m_CurrentJitter = GenerateHaltonJitter(m_JitterFrameIndex) * 2.0f - 1.0f;

    JitteredProjection[2][0] += m_CurrentJitter.x / Viewport.X;
    JitteredProjection[2][1] += m_CurrentJitter.y / Viewport.Y;
  }
  glm::mat4 JitteredViewProjection = JitteredProjection * View;

  return TRenderContext{
      .SceneRenderTarget            = *m_SceneTarget,
      .PostProcessRenderTarget      = *m_PostProcessTarget,
      .FinalRenderTarget            = m_FinalTarget.get(),
      .CameraPosition               = Camera->GetPosition(),
      .ProjectionMatrix             = Projection,
      .ViewMatrix                   = View,
      .ViewProjectionMatrix         = Projection * View,
      .JitteredViewProjectionMatrix = JitteredViewProjection,
      .PreviousViewProjectionMatrix = m_PrevViewProjectionMatrix,
      .LightSpaceMatrix             = CalculateLightSpaceMatrix(),
      .Jitter                       = m_CurrentJitter,
      .PrevJitter                   = m_PreviousJitter,
      .ShadowMap                    = 0,
      .IrradianceMap                = FrameData.Environment.IrradianceMap,
      .TAAHistoryMap                = 0,
      .QuadVAO                      = m_QuadBuffer.VAO,
      .CubeVAO                      = m_CubeBuffer.VAO,
  };
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

std::shared_ptr<CTextureBase> CRenderPipeline::CreateDepthTexture(const std::string &_Name, TVector2i _Size)
{
  TTextureParams TextureParams;
  TextureParams.Width          = _Size.X;
  TextureParams.Height         = _Size.Y;
  TextureParams.InternalFormat = GL_DEPTH_COMPONENT24;
  TextureParams.Format         = GL_DEPTH_COMPONENT;
  TextureParams.Type           = GL_FLOAT;
  TextureParams.MinFilter      = ETextureFilter::Nearest;
  TextureParams.MagFilter      = ETextureFilter::Nearest;
  TextureParams.WrapS          = ETextureWrap::ClampToEdge;
  TextureParams.WrapT          = ETextureWrap::ClampToEdge;

  return resource::RecreateTexture(_Name, TextureParams);
}

std::shared_ptr<CTextureBase> CRenderPipeline::CreateVelocityTexture(const std::string &_Name, TVector2i _Size)
{
  TTextureParams TextureParams;
  TextureParams.Width          = _Size.X;
  TextureParams.Height         = _Size.Y;
  TextureParams.InternalFormat = GL_RG16F;
  TextureParams.Format         = GL_RG;
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

void CRenderPipeline::InitRenderTargets(TVector2i _Viewport)
{
  const auto CreateDepthRBO = [](TVector2i _Size) {
    CRenderBuffer DepthRBO;
    DepthRBO.Bind();
    DepthRBO.AllocateStorage(GL_DEPTH_COMPONENT, _Size.X, _Size.Y);
    DepthRBO.Unbind();
    return DepthRBO;
  };

  m_SceneTarget           = std::make_unique<TRenderTarget>();
  m_SceneTarget->Size     = _Viewport;
  m_SceneTarget->Color    = CreateRenderTexture("SCENE_TARGET_COLOR", _Viewport);
  m_SceneTarget->Depth    = CreateDepthTexture("SCENE_TARGET_DEPTH", _Viewport);
  m_SceneTarget->Velocity = CreateVelocityTexture("SCENE_TARGET_VELOCITY", _Viewport);
  m_SceneTarget->FrameBuffer.Bind();
  m_SceneTarget->FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, m_SceneTarget->Color->ID());
  m_SceneTarget->FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT1, m_SceneTarget->Velocity->ID());
  m_SceneTarget->FrameBuffer.AttachTexture(GL_DEPTH_ATTACHMENT, std::get<TRenderTarget::TTexture>(m_SceneTarget->Depth)->ID());
  m_SceneTarget->FrameBuffer.Unbind();

  m_PostProcessTarget        = std::make_unique<TRenderTarget>();
  m_PostProcessTarget->Size  = _Viewport;
  m_PostProcessTarget->Color = CreateRenderTexture("POST_PROCESS_TARGET_COLOR", _Viewport);
  m_PostProcessTarget->Depth = CreateDepthRBO(_Viewport);
  m_PostProcessTarget->FrameBuffer.Bind();
  m_PostProcessTarget->FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, m_PostProcessTarget->Color->ID());
  m_PostProcessTarget->FrameBuffer.AttachRenderBuffer(GL_DEPTH_ATTACHMENT, std::get<CRenderBuffer>(m_PostProcessTarget->Depth).ID());
  m_PostProcessTarget->FrameBuffer.Unbind();

  if (CConfig::Instance().IsEditorEnabled())
  {
    m_FinalTarget        = std::make_unique<TRenderTarget>();
    m_FinalTarget->Size  = _Viewport;
    m_FinalTarget->Color = CreateRenderTexture("FINAL_TARGET_COLOR", _Viewport);
    m_FinalTarget->Depth = CreateDepthRBO(_Viewport);
    m_FinalTarget->FrameBuffer.Bind();
    m_FinalTarget->FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, m_FinalTarget->Color->ID());
    m_FinalTarget->FrameBuffer.AttachRenderBuffer(GL_DEPTH_ATTACHMENT, std::get<CRenderBuffer>(m_FinalTarget->Depth).ID());
    m_FinalTarget->FrameBuffer.Unbind();
  }
}

void CRenderPipeline::InitCommonVAOs()
{
  m_QuadBuffer.VAO.Bind();
  m_QuadBuffer.VBO.Bind();
  m_QuadBuffer.VBO.Assign(QUAD_VERTICES, sizeof(QUAD_VERTICES));
  m_QuadBuffer.VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 5 * sizeof(float));
  m_QuadBuffer.VAO.EnableAttrib(ATTRIB_LOC_TEXCOORDS_0, 2, GL_FLOAT, false, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  m_QuadBuffer.VAO.Unbind();

  m_CubeBuffer.VAO.Bind();
  m_CubeBuffer.VBO.Bind();
  m_CubeBuffer.VBO.Assign(CUBE_VERTICES, sizeof(CUBE_VERTICES));
  m_CubeBuffer.VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 3 * sizeof(float));
  m_CubeBuffer.VAO.Unbind();
}

glm::vec2 CRenderPipeline::GenerateHaltonJitter(uint32_t _Index)
{
  auto HaltonSequence = [](uint32_t Index, uint32_t Base) -> float {
    float Fraction = 1.0f;
    float Result   = 0.0f;

    while (Index > 0)
    {
      Fraction /= Base;
      Result   += Fraction * (Index % Base);
      Index    /= Base;
    }
    return Result;
  };

  return glm::vec2(HaltonSequence(_Index % 8, 2), HaltonSequence(_Index % 8, 3));
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

float CRenderPipeline::GetRenderPassTime(ERenderPassType _Type) const
{
  auto It = m_RenderPassTimes.find(_Type);
  if (It != m_RenderPassTimes.end())
    return It->second;

  return 0.0f;
}
