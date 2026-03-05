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
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Camera.h"
#include "engine/Config.h"
#include "engine/Engine.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "utils/Resource.h"
#include "utils/Logger.h"
#include <glm/gtx/norm.hpp>

const std::string CRenderPipeline::RENDER_TEXTURE_NAME = "PIPELINE_SCENE_RENDER_TEXTURE";

CRenderPipeline::CRenderPipeline() :
    m_Lighting({}),
    m_LightingUBO(GL_DYNAMIC_DRAW)
{
}

CRenderPipeline::~CRenderPipeline() = default;

void CRenderPipeline::Shutdown()
{
  m_SceneTarget.reset();
  m_PostProcessTarget.reset();
}

void CRenderPipeline::Init()
{
  const bool ShadowsEnabled = CConfig::Instance().GetShadowsEnabled();
  if (ShadowsEnabled)
    m_ShadowPasses.push_back(CShadowRenderPass::Create(resource::LoadShader("Depth")));

  m_GeometryPasses.push_back(COpaqueRenderPass::Create(resource::LoadShader("PBR")));
  m_GeometryPasses.push_back(CSkyboxRenderPass::Create(resource::LoadShader("Skybox")));
  m_GeometryPasses.push_back(CTransparentRenderPass::Create(resource::LoadShader("PBR")));
  m_PostProcessPasses.push_back(CPostProcessRenderPass::Create(resource::LoadShader("PostProcess")));
  m_OutputPasses.push_back(COutputRenderPass::Create(resource::LoadShader("Output")));

  m_SceneTarget       = CreateRenderTarget(RENDER_TEXTURE_NAME + "0", CEngine::Instance().GetViewportSize());
  m_PostProcessTarget = CreateRenderTarget(RENDER_TEXTURE_NAME + "1", CEngine::Instance().GetViewportSize());

  m_LightingUBO.Bind();
  m_LightingUBO.Reserve(sizeof(TShaderLighting));
  m_LightingUBO.BindToBase(BINDING_LIGHTING_BUFFER);
  m_LightingUBO.Unbind();

  event::Subscribe(TEventType::ViewportResized, GetWeakPtr());
  event::Subscribe(TEventType::Config_ShadowsEnabledChanged, GetWeakPtr());
}

void CRenderPipeline::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::Config_ShadowsEnabledChanged: {
    const bool ShadowsEnabled       = !m_ShadowPasses.empty();
    const bool NeedToEnableShadows  = _Event.GetValue<bool>() && !ShadowsEnabled;
    const bool NeedToDisableShadows = !_Event.GetValue<bool>() && ShadowsEnabled;

    if (NeedToEnableShadows)
      m_ShadowPasses.push_back(CShadowRenderPass::Create(resource::LoadShader("Depth")));
    else if (NeedToDisableShadows)
      m_ShadowPasses.clear();

    break;
  }
  case TEventType::ViewportResized: {
    const TVector2i NewSize = _Event.GetValue<TVector2i>();

    m_SceneTarget       = CreateRenderTarget(RENDER_TEXTURE_NAME + "0", NewSize);
    m_PostProcessTarget = CreateRenderTarget(RENDER_TEXTURE_NAME + "1", NewSize);
    break;
  }
  }
}

void CRenderPipeline::Render(TFrameData &FrameData, CRenderQueue &_Queue, IRenderer &_Renderer)
{
  BeginFrame(_Renderer);

  if (std::vector<TRenderCommand> Commands = _Queue.StealCommands(); !Commands.empty())
  {
    SetLightingData(FrameData.Lights);

    TRenderContext RenderContext = CreateRenderContext(_Renderer);
    SortCommands(Commands, RenderContext);

    ShadowPass(_Renderer, RenderContext, Commands);
    GeometryPass(_Renderer, RenderContext, Commands);
    PostProcessPass(_Renderer, RenderContext, Commands);
    //OutputPass(_Renderer, RenderContext, Commands); // To render without editor. Untested
  }

  EndFrame(_Renderer);
}

void CRenderPipeline::BeginFrame(IRenderer &_Renderer)
{
  _Renderer.SetViewport(CEngine::Instance().GetViewportSize());
  _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));
  _Renderer.ClearColor({0.86f, 0.86f, 0.86f, 1.0f});
}

void CRenderPipeline::EndFrame(IRenderer &_Renderer)
{
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

void CRenderPipeline::OutputPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  // glEnable(GL_FRAMEBUFFER_SRGB); verify if needed

  _RenderContext.PostProcessRenderTarget.FrameBuffer.Unbind();

  for (const std::shared_ptr<IRenderPass> &RenderPass : m_OutputPasses)
    DoRenderPass(RenderPass, _Renderer, _RenderContext, _Commands);
}

void CRenderPipeline::DoRenderPass(const std::shared_ptr<IRenderPass> &_RenderPass,
                                   IRenderer                          &_Renderer,
                                   TRenderContext                     &_RenderContext,
                                   std::vector<TRenderCommand>        &_Commands)
{

  if (!_RenderPass->IsAvailable())
    return;

  std::span<TRenderCommand> CommandsSpan = FilterCommands(_RenderPass, _Commands);
  if (CommandsSpan.empty())
    return;

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

uint32_t CRenderPipeline::GetRenderTextureID() const
{
  return m_PostProcessTarget ? m_PostProcessTarget->Color->ID() : 0;
}