#include "RenderPipeline.h"
#include "RenderQueue.h"
#include "passes/OpaqueRenderPass.h"
#include "passes/SkyboxRenderPass.h"
#include "passes/TransparentRenderPass.h"
#include "passes/ShadowRenderPass.h"
#include "FrameContext.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "engine/Camera.h"
#include "engine/Config.h"
#include "interfaces/Renderer.h"
#include "render/FrameContext.h"
#include "utils/Resource.h"
#include <glm/gtx/norm.hpp>

CRenderPipeline::CRenderPipeline() :
    m_Lighting({}),
    m_LightingUBO(GL_DYNAMIC_DRAW),
    m_LightSpaceMatrix(1.0f),
    m_LightSpaceMatrixDirty(true)
{
}

CRenderPipeline::~CRenderPipeline() = default;

void CRenderPipeline::Init()
{
  m_RenderPasses.push_back(std::make_unique<ShadowRenderPass>(resource::LoadShader("Depth")));
  m_RenderPasses.push_back(std::make_unique<OpaqueRenderPass>(resource::LoadShader("Basic")));
  m_RenderPasses.push_back(std::make_unique<SkyboxRenderPass>(resource::LoadShader("Skybox")));
  m_RenderPasses.push_back(std::make_unique<TransparentRenderPass>(resource::LoadShader("Basic")));

  m_LightingUBO.Bind();
  m_LightingUBO.Reserve(sizeof(TShaderLighting));
  m_LightingUBO.BindToBase(BINDING_LIGHTING_BUFFER);
  m_LightingUBO.Unbind();
}

void CRenderPipeline::Render(CRenderQueue &_Queue, IRenderer &_Renderer)
{
  std::vector<TRenderCommand> Commands = _Queue.StealCommands();
  if (Commands.empty())
    return;

  TFrameContext FrameContext = CreateFrameContext(_Renderer);
  SortCommands(Commands, FrameContext);

  for (const std::unique_ptr<IRenderPass> &RenderPass : m_RenderPasses)
  {
    if (!RenderPass->IsAvailable())
      continue;

    std::span<TRenderCommand> CommandsSpan = FilterCommands(RenderPass, Commands);
    if (CommandsSpan.empty())
      continue;

    RenderPass->PreExecute(_Renderer, FrameContext, CommandsSpan);
    RenderPass->Execute(_Renderer, FrameContext, CommandsSpan);
    RenderPass->PostExecute(_Renderer, FrameContext, CommandsSpan);
  }
}

void CRenderPipeline::SortCommands(std::vector<TRenderCommand> &_Commands, const TFrameContext &_FrameContext)
{
  auto It = std::stable_partition(_Commands.begin(), _Commands.end(), [](const TRenderCommand &Command) {
    return Command.Material.AlphaMode != EAlphaMode::Blend;
  });

  const glm::vec3 CamPos = _FrameContext.CameraPosition;

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
  m_LightSpaceMatrixDirty = true;
}

glm::mat4 CRenderPipeline::CalculateLightSpaceMatrix() const
{
  if (m_LightSpaceMatrixDirty)
  {
    const float NearPlane = CConfig::Instance().GetLightSpaceMatrixZNear();
    const float FarPlane  = CConfig::Instance().GetLightSpaceMatrixZFar();
    const float LeftBot   = CConfig::Instance().GetLightSpaceMatrixOrthLeftBot();
    const float RightTop  = CConfig::Instance().GetLightSpaceMatrixOrthRightTop();

    const glm::vec3 LightDir        = m_Lighting.LightDirectional.Direction * -1.0f;
    const glm::mat4 LightProjection = glm::ortho(LeftBot, RightTop, LeftBot, RightTop, NearPlane, FarPlane);
    const glm::mat4 LightView       = glm::lookAt(LightDir, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

    m_LightSpaceMatrix      = LightProjection * LightView;
    m_LightSpaceMatrixDirty = false;
  }
  return m_LightSpaceMatrix;
}

TFrameContext CRenderPipeline::CreateFrameContext(IRenderer &_Renderer)
{
  TFrameContext FrameContext;
  FrameContext.CameraPosition       = _Renderer.GetCamera()->GetPosition();
  FrameContext.ProjectionMatrix     = _Renderer.GetCamera()->GetProjection();
  FrameContext.ViewMatrix           = _Renderer.GetCamera()->GetView();
  FrameContext.ViewProjectionMatrix = FrameContext.ProjectionMatrix * FrameContext.ViewMatrix;
  FrameContext.LightSpaceMatrix     = CalculateLightSpaceMatrix();
  return FrameContext;
}