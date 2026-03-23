#include "PrepareEnvPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "render/RenderTarget.h"
#include "render/ShaderTypes.h"
#include "render/Buffer.h"
#include "assets/Texture.h"
#include "utils/Resource.h"
#include <glm/gtc/matrix_transform.hpp>

CPrepareEnvPass::CPrepareEnvPass() :
    m_EquirectToCubemapShader(resource::LoadShader("EquirectangularToCubemap")),
    m_IrradianceShader(resource::LoadShader("IrradianceConvolution")),
    m_PrefilterShader(resource::LoadShader("Prefilter")),
    m_BRDFLUTShader(resource::LoadShader("BRDF"))
{
  m_Projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  m_Views[0]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_Views[1]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_Views[2]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  m_Views[3]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
  m_Views[4]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_Views[5]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

void CPrepareEnvPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthFunc(GL_LEQUAL);
  _Renderer.SetCullFace(ECullMode::None);
}

void CPrepareEnvPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  const TRenderCommand *Command = _Commands.front();
  ConvertEquirectangularToCubemap(_Renderer, _RenderContext, *Command);
  IrradianceConvolution(_Renderer, _RenderContext, *Command);
  CreatePrefilterMap(_Renderer, _RenderContext, *Command);
  CreateBRDFLUT(_Renderer, _RenderContext, *Command);
}

void CPrepareEnvPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  CFrameBuffer::BindDefault();
}

bool CPrepareEnvPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.RenderFlags.test(ERenderFlags_PrepareEnvironment);
}

bool CPrepareEnvPass::IsAvailable() const
{
  return m_EquirectToCubemapShader && m_IrradianceShader && m_PrefilterShader && m_BRDFLUTShader;
}

bool CPrepareEnvPass::NeedsCommands() const
{
  return true;
}

void CPrepareEnvPass::ConvertEquirectangularToCubemap(IRenderer &_Renderer, TRenderContext &_RenderContext, const TRenderCommand &_Command)
{
  _RenderContext.CubeVAO.Bind();

  m_FBO.Bind();
  m_RBO.Bind();
  m_RBO.AllocateStorage(GL_DEPTH_COMPONENT24, CUBEMAP_SIZE, CUBEMAP_SIZE);
  m_FBO.AttachRenderBuffer(GL_DEPTH_ATTACHMENT, m_RBO.ID());

  _Renderer.SetShader(m_EquirectToCubemapShader);
  _Renderer.SetViewport({CUBEMAP_SIZE, CUBEMAP_SIZE});
  _Renderer.SetUniform("u_Projection", m_Projection);

  C2DTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, _Command.Environment.EquirectangularMap);
  _Renderer.SetUniform("u_EquirectangularMap", TEXTURE_BASIC_COLOR_INDEX);

  for (int i = 0; i < CUBEMAP_FACES; ++i)
  {
    m_FBO.AttachTexture(GL_COLOR_ATTACHMENT0, _Command.Environment.SkyboxTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
    _Renderer.SetUniform("u_View", m_Views[i]);
    _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));
    _Renderer.DrawArrays(EPrimitiveMode::Triangles, ARRAY_SIZE(CUBE_VERTICES) / 3);
  }

  CCubemap::GenerateMipmaps(_Command.Environment.SkyboxTexture);

  _RenderContext.CubeVAO.Unbind();
}

void CPrepareEnvPass::IrradianceConvolution(IRenderer &_Renderer, TRenderContext &_RenderContext, const TRenderCommand &_Command)
{
  _RenderContext.CubeVAO.Bind();

  m_FBO.Bind();
  m_RBO.Bind();
  m_RBO.AllocateStorage(GL_DEPTH_COMPONENT24, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE);
  m_FBO.AttachRenderBuffer(GL_DEPTH_ATTACHMENT, m_RBO.ID());

  _Renderer.SetShader(m_IrradianceShader);
  _Renderer.SetViewport({IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE});
  _Renderer.SetUniform("u_Projection", m_Projection);

  CCubemap::Bind(TEXTURE_SKYBOX_UNIT, _Command.Environment.SkyboxTexture);
  _Renderer.SetUniform("u_EnvironmentMap", TEXTURE_SKYBOX_INDEX);

  for (int i = 0; i < CUBEMAP_FACES; ++i)
  {
    m_FBO.AttachTexture(GL_COLOR_ATTACHMENT0, _Command.Environment.IrradianceMap, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
    _Renderer.SetUniform("u_View", m_Views[i]);
    _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));
    _Renderer.DrawArrays(EPrimitiveMode::Triangles, ARRAY_SIZE(CUBE_VERTICES) / 3);
  }

  _RenderContext.CubeVAO.Unbind();
}

void CPrepareEnvPass::CreatePrefilterMap(IRenderer &_Renderer, TRenderContext &_RenderContext, const TRenderCommand &_Command)
{
  _RenderContext.CubeVAO.Bind();

  m_FBO.Bind();
  m_RBO.Bind();

  _Renderer.SetShader(m_PrefilterShader);
  _Renderer.SetUniform("u_Projection", m_Projection);
  _Renderer.SetUniform("u_EnvironmentMap", TEXTURE_SKYBOX_INDEX);
  CCubemap::Bind(TEXTURE_SKYBOX_UNIT, _Command.Environment.SkyboxTexture);

  for (int Mip = 0; Mip < MIP_LEVELS; ++Mip)
  {
    const unsigned MapWidth  = static_cast<unsigned int>(128 * std::pow(0.5, Mip));
    const unsigned MapHeight = static_cast<unsigned int>(128 * std::pow(0.5, Mip));
    m_RBO.AllocateStorage(GL_DEPTH_COMPONENT24, MapWidth, MapHeight);
    _Renderer.SetViewport({MapWidth, MapHeight});

    const float Roughness = Mip / static_cast<float>(MIP_LEVELS - 1);
    _Renderer.SetUniform("u_Roughness", Roughness);
    for (int i = 0; i < CUBEMAP_FACES; ++i)
    {
      _Renderer.SetUniform("u_View", m_Views[i]);
      m_FBO.AttachTexture(GL_COLOR_ATTACHMENT0, _Command.Environment.PrefilteredMap, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, Mip);

      _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));
      _Renderer.DrawArrays(EPrimitiveMode::Triangles, ARRAY_SIZE(CUBE_VERTICES) / 3);
    }
  }

  _RenderContext.CubeVAO.Unbind();
}

void CPrepareEnvPass::CreateBRDFLUT(IRenderer &_Renderer, TRenderContext &_RenderContext, const TRenderCommand &_Command)
{
  _RenderContext.QuadVAO.Bind();

  m_FBO.Bind();
  m_RBO.Bind();
  m_RBO.AllocateStorage(GL_DEPTH_COMPONENT24, BRDF_LUT_SIZE, BRDF_LUT_SIZE);

  _Renderer.SetShader(m_BRDFLUTShader);
  _Renderer.SetViewport({BRDF_LUT_SIZE, BRDF_LUT_SIZE});

  m_FBO.AttachTexture(GL_COLOR_ATTACHMENT0, _Command.Environment.BRDFLUT);

  _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);

  _RenderContext.QuadVAO.Unbind();
}
