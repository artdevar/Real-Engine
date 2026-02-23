#include "RenderPipeline.h"
#include "RenderQueue.h"
#include "FrameData.h"
#include "passes/OpaqueRenderPass.h"
#include "passes/SkyboxRenderPass.h"
#include "passes/TransparentRenderPass.h"
#include "passes/ShadowRenderPass.h"
#include "passes/PostProcessRenderPass.h"
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

CRenderPipeline::CRenderPipeline() :
    m_Lighting({}),
    m_LightingUBO(GL_DYNAMIC_DRAW)
{
}

CRenderPipeline::~CRenderPipeline() = default;

void CRenderPipeline::Init()
{
  m_ShadowPasses.push_back(std::make_unique<ShadowRenderPass>(resource::LoadShader("Depth")));
  m_GeometryPasses.push_back(std::make_unique<OpaqueRenderPass>(resource::LoadShader("PBR")));
  m_GeometryPasses.push_back(std::make_unique<SkyboxRenderPass>(resource::LoadShader("Skybox")));
  m_GeometryPasses.push_back(std::make_unique<TransparentRenderPass>(resource::LoadShader("PBR")));
  m_PostProcessPasses.push_back(std::make_unique<PostProcessRenderPass>(resource::LoadShader("PostProcess")));

  const TVector2i WindowSize       = CEngine::Instance().GetWindowSize();
  const int       MultisampleCount = CConfig::Instance().GetMultisampleCount();

  m_SceneFBO.Texture = CreateRenderTexture("Scene", MultisampleCount, WindowSize);

  m_SceneFBO.RenderBuffer.Bind();
  m_SceneFBO.RenderBuffer.AllocateStorage(GL_DEPTH_COMPONENT, WindowSize.X, WindowSize.Y);
  m_SceneFBO.RenderBuffer.Unbind();

  m_SceneFBO.FrameBuffer.Bind();
  m_SceneFBO.FrameBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, m_SceneFBO.Texture->ID());
  m_SceneFBO.FrameBuffer.AttachRenderbuffer(GL_DEPTH_ATTACHMENT, m_SceneFBO.RenderBuffer.ID());
  m_SceneFBO.FrameBuffer.Unbind();

  m_LightingUBO.Bind();
  m_LightingUBO.Reserve(sizeof(TShaderLighting));
  m_LightingUBO.BindToBase(BINDING_LIGHTING_BUFFER);
  m_LightingUBO.Unbind();
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
  }

  EndFrame(_Renderer);
}

void CRenderPipeline::BeginFrame(IRenderer &_Renderer)
{
  _Renderer.SetViewport(CEngine::Instance().GetWindowSize());
  _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));
  _Renderer.ClearColor({0.86f, 0.86f, 0.86f, 1.0f});
}

void CRenderPipeline::EndFrame(IRenderer &_Renderer)
{
  _Renderer.CheckErrors();
}

void CRenderPipeline::ShadowPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  _RenderContext.SceneFrameBuffer.get().Unbind();

  for (const std::unique_ptr<IRenderPass> &RenderPass : m_ShadowPasses)
    DoRenderPass(RenderPass, _Renderer, _RenderContext, _Commands);
}

void CRenderPipeline::GeometryPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  _RenderContext.SceneFrameBuffer.get().Bind();
  BeginFrame(_Renderer);

  for (const std::unique_ptr<IRenderPass> &RenderPass : m_GeometryPasses)
    DoRenderPass(RenderPass, _Renderer, _RenderContext, _Commands);

  _RenderContext.SceneFrameBuffer.get().Unbind();
}

void CRenderPipeline::PostProcessPass(IRenderer &_Renderer, TRenderContext &_RenderContext, std::vector<TRenderCommand> &_Commands)
{
  _RenderContext.SceneFrameBuffer.get().Unbind();
  BeginFrame(_Renderer);

  for (const std::unique_ptr<IRenderPass> &RenderPass : m_PostProcessPasses)
    DoRenderPass(RenderPass, _Renderer, _RenderContext, _Commands);
}

void CRenderPipeline::DoRenderPass(const std::unique_ptr<IRenderPass> &_RenderPass,
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

std::span<TRenderCommand> CRenderPipeline::FilterCommands(const std::unique_ptr<IRenderPass> &_RenderPass, std::vector<TRenderCommand> &_Commands)
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
          .Ambient   = Light.Ambient,
          .Diffuse   = Light.Diffuse,
          .Specular  = Light.Specular,
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
      .SceneFrameBuffer     = m_SceneFBO.FrameBuffer,
      .RenderTexture        = m_SceneFBO.Texture->ID(),
      .CameraPosition       = Camera->GetPosition(),
      .ProjectionMatrix     = Camera->GetProjection(),
      .ViewMatrix           = Camera->GetView(),
      .ViewProjectionMatrix = Camera->GetProjection() * Camera->GetView(),
      .LightSpaceMatrix     = CalculateLightSpaceMatrix(),
  };
}

std::shared_ptr<CTextureBase> CRenderPipeline::CreateRenderTexture(const std::string &_Name, int _MultisampleCount, TVector2i _Size) const
{
  TTextureParams TextureParams;
  TextureParams.Width          = _Size.X;
  TextureParams.Height         = _Size.Y;
  TextureParams.InternalFormat = GL_RGBA16F;
  TextureParams.Format         = GL_RGBA;
  TextureParams.Type           = GL_FLOAT;
  TextureParams.Samples        = _MultisampleCount;
  TextureParams.MinFilter      = ETextureFilter::Linear;
  TextureParams.MagFilter      = ETextureFilter::Linear;
  return resource::CreateTexture(_Name, TextureParams);
}