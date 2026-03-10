#include "EquirectangularToCubemapPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "render/RenderTarget.h"
#include "render/ShaderTypes.h"
#include "render/Buffer.h"
#include "assets/Texture.h"
#include "utils/Resource.h"
#include <glm/gtc/matrix_transform.hpp>

static constexpr float CUBE_VERTICES[] = {-1.0f, 1.0f,  -1.0f, //
                                          -1.0f, -1.0f, -1.0f, //
                                          1.0f,  -1.0f, -1.0f, //
                                          1.0f,  -1.0f, -1.0f, //
                                          1.0f,  1.0f,  -1.0f, //
                                          -1.0f, 1.0f,  -1.0f, //

                                          -1.0f, -1.0f, 1.0f,  //
                                          -1.0f, -1.0f, -1.0f, //
                                          -1.0f, 1.0f,  -1.0f, //
                                          -1.0f, 1.0f,  -1.0f, //
                                          -1.0f, 1.0f,  1.0f,  //
                                          -1.0f, -1.0f, 1.0f,  //

                                          1.0f,  -1.0f, -1.0f, //
                                          1.0f,  -1.0f, 1.0f,  //
                                          1.0f,  1.0f,  1.0f,  //
                                          1.0f,  1.0f,  1.0f,  //
                                          1.0f,  1.0f,  -1.0f, //
                                          1.0f,  -1.0f, -1.0f, //

                                          -1.0f, -1.0f, 1.0f, //
                                          -1.0f, 1.0f,  1.0f, //
                                          1.0f,  1.0f,  1.0f, //
                                          1.0f,  1.0f,  1.0f, //
                                          1.0f,  -1.0f, 1.0f, //
                                          -1.0f, -1.0f, 1.0f, //

                                          -1.0f, 1.0f,  -1.0f, //
                                          1.0f,  1.0f,  -1.0f, //
                                          1.0f,  1.0f,  1.0f,  //
                                          1.0f,  1.0f,  1.0f,  //
                                          -1.0f, 1.0f,  1.0f,  //
                                          -1.0f, 1.0f,  -1.0f, //

                                          -1.0f, -1.0f, -1.0f, //
                                          -1.0f, -1.0f, 1.0f,  //
                                          1.0f,  -1.0f, -1.0f, //
                                          1.0f,  -1.0f, -1.0f, //
                                          -1.0f, -1.0f, 1.0f,  //
                                          1.0f,  -1.0f, 1.0f};

CEquirectangularToCubemapPass::CEquirectangularToCubemapPass() :
    m_Shader(resource::LoadShader("EquirectangularToCubemap")),
    m_VAO(),
    m_VBO(GL_STATIC_DRAW)
{
  m_VAO.Bind();
  m_VBO.Bind();
  m_VBO.Assign(CUBE_VERTICES, sizeof(CUBE_VERTICES));
  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 3 * sizeof(float));
  m_VAO.Unbind();
  m_VBO.Unbind();

  m_FBO.Bind();
  m_RBO.Bind();
  m_RBO.AllocateStorage(GL_DEPTH_COMPONENT24, 512, 512);
  m_FBO.AttachRenderbuffer(GL_DEPTH_ATTACHMENT, m_RBO.ID());

  m_Projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  m_Views[0]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_Views[1]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_Views[2]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  m_Views[3]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
  m_Views[4]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_Views[5]   = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

void CEquirectangularToCubemapPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthFunc(GL_LEQUAL);
  _Renderer.SetCullFace(ECullMode::None);
  _Renderer.SetViewport({512, 512});
  _Renderer.SetShader(m_Shader);
}

void CEquirectangularToCubemapPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  m_VAO.Bind();
  _Renderer.SetUniform("u_Projection", m_Projection);

  for (const TRenderCommand &Command : _Commands)
  {
    CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, Command.Material.BaseColorTexture);
    _Renderer.SetUniform("u_EquirectangularMap", TEXTURE_BASIC_COLOR_INDEX);

    m_FBO.Bind();
    for (int i = 0; i < CUBEMAP_FACES; ++i)
    {
      m_FBO.AttachTexture(GL_COLOR_ATTACHMENT0, Command.Material.SkyboxTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
      _Renderer.SetUniform("u_View", m_Views[i]);
      _Renderer.Clear(static_cast<EClearFlags>(EClearFlags::Color | EClearFlags::Depth));
      _Renderer.DrawArrays(EPrimitiveMode::Triangles, ARRAY_SIZE(CUBE_VERTICES) / 3);
    }
  }

  m_VAO.Unbind();
}

void CEquirectangularToCubemapPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _Renderer.SetDepthFunc(GL_LESS);
}

bool CEquirectangularToCubemapPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.RenderFlags.test(ERenderFlags_EquirectangularToCubemap);
}

bool CEquirectangularToCubemapPass::IsAvailable() const
{
  return m_Shader != nullptr;
}
