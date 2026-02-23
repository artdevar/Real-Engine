#include "ShadowRenderPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "interfaces/Renderer.h"
#include "assets/Texture.h"
#include "engine/Config.h"
#include "utils/Resource.h"

ShadowRenderPass::ShadowRenderPass(std::shared_ptr<CShader> _Shader) :
    m_ShadowMapSize(CConfig::Instance().GetShadowMapSize()),
    m_Shader(std::move(_Shader)),
    m_DepthMap(CreateDepthMap(TVector2i(m_ShadowMapSize, m_ShadowMapSize))),
    m_DepthMapFBO()
{
  assert(m_DepthMap);
  m_DepthMapFBO.Bind();
  m_DepthMapFBO.AttachTexture(GL_DEPTH_ATTACHMENT, m_DepthMap->ID());
  m_DepthMapFBO.DisableColorBuffer();
  m_DepthMapFBO.Unbind();
}

void ShadowRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  m_OldViewport = _Renderer.GetViewport();
  TVector2i NewViewport(m_ShadowMapSize, m_ShadowMapSize);

  _RenderContext.SceneFrameBuffer.get().Unbind();
  m_DepthMapFBO.Bind();

  _Renderer.Clear(EClearFlags::Depth);
  _Renderer.SetCullFace(ECullMode::Front);
  _Renderer.SetViewport(NewViewport);
  _Renderer.SetShader(m_Shader.lock());
  _Renderer.SetUniform("u_LightSpaceMatrix", _RenderContext.LightSpaceMatrix);
}

void ShadowRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  for (const TRenderCommand &Command : _Commands)
  {
    _Renderer.SetUniform("u_Model", Command.ModelMatrix);

    CTexture::Bind(TEXTURE_BASIC_COLOR_UNIT, Command.Material.BaseColorTexture);
    _Renderer.SetUniform("u_Material.BaseColorTexture", TEXTURE_BASIC_COLOR_INDEX);
    _Renderer.SetUniform("u_Material.AlphaCutoff", Command.Material.AlphaCutoff);
    _Renderer.SetUniform("u_Material.AlphaMode", static_cast<int>(Command.Material.AlphaMode));

    Command.VAO.get().Bind();

    if (Command.IndexType != EIndexType::None)
      _Renderer.DrawElements(Command.PrimitiveMode, Command.IndicesCount, Command.IndexType);
    else
      _Renderer.DrawArrays(Command.PrimitiveMode, Command.IndicesCount);

    Command.VAO.get().Unbind();
  }
}

void ShadowRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, std::span<TRenderCommand> _Commands)
{
  _RenderContext.ShadowMap = m_DepthMap->ID();

  m_DepthMapFBO.Unbind();
  _RenderContext.SceneFrameBuffer.get().Bind();

  _Renderer.SetCullFace(ECullMode::Back);
  _Renderer.SetViewport(m_OldViewport);
}

bool ShadowRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return _Command.Material.SkyboxTexture == CCubemap::INVALID_VALUE && _Command.Material.BaseColorTexture != CTexture::INVALID_VALUE;
}

bool ShadowRenderPass::IsAvailable() const
{
  return !m_Shader.expired() && m_DepthMap && m_DepthMap->IsValid();
}

std::shared_ptr<CTextureBase> ShadowRenderPass::CreateDepthMap(TVector2i _Size)
{
  TTextureParams DepthMapParams;
  DepthMapParams.BorderColors.emplace({1.0f, 1.0f, 1.0f, 1.0f});
  DepthMapParams.Width          = _Size.X;
  DepthMapParams.Height         = _Size.Y;
  DepthMapParams.InternalFormat = GL_DEPTH_COMPONENT16;
  DepthMapParams.Format         = GL_DEPTH_COMPONENT;
  DepthMapParams.Type           = GL_FLOAT;
  DepthMapParams.WrapS          = ETextureWrap::ClampToBorder;
  DepthMapParams.WrapT          = ETextureWrap::ClampToBorder;
  DepthMapParams.MinFilter      = ETextureFilter::Nearest;
  DepthMapParams.MagFilter      = ETextureFilter::Nearest;
  return resource::CreateTexture("ShadowDepthMap", DepthMapParams);
}

#if 0
void ShadowRenderPass::RenderDebugQuad(IRenderer &_Renderer)
{
  static CVertexArray VAO = [this]() {
    constexpr float QuadVertices[] = {// positions     // texcoords

                                      -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
                                      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
                                      1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //

                                      -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
                                      1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //
                                      1.0f,  1.0f,  0.0f, 1.0f, 1.0f};

    CVertexArray  VAO;
    CVertexBuffer VBO(GL_STATIC_DRAW);
    VAO.Bind();
    VBO.Bind();
    VBO.Assign(QuadVertices, sizeof(QuadVertices));
    VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 5 * sizeof(float), (void *)0);
    VAO.EnableAttrib(ATTRIB_LOC_TEXCOORDS_0, 2, GL_FLOAT, false, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    VAO.Unbind();

    return VAO;
  }();

  static std::shared_ptr<CShader> DebugShader = resource::LoadShader("DebugDepth");
  _Renderer.SetShader(DebugShader);

  m_DepthMap->Bind(GL_TEXTURE0);
  _Renderer.SetUniform("u_DepthMap", 0);
  _Renderer.SetUniform("u_NearPlane", CConfig::Instance().GetLightSpaceMatrixZNear());
  _Renderer.SetUniform("u_FarPlane", CConfig::Instance().GetLightSpaceMatrixZFar());

  VAO.Bind();
  _Renderer.DrawArrays(EPrimitiveMode::Triangles, 6);
  VAO.Unbind();
}
#endif