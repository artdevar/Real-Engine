#include "GridRenderPass.h"
#include "interfaces/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderCommand.h"
#include "render/RenderTarget.h"
#include "assets/Shader.h"
#include "utils/Resource.h"
#include <vector>

namespace
{

std::vector<float> GenerateGridVertices(float _Extent, float _Step)
{
  std::vector<float> Vertices;
  // Horizontal lines (parallel to X axis)
  for (float z = -_Extent; z <= _Extent; z += _Step)
  {
    Vertices.push_back(-_Extent);
    Vertices.push_back(0.0f);
    Vertices.push_back(z);

    Vertices.push_back(_Extent);
    Vertices.push_back(0.0f);
    Vertices.push_back(z);
  }

  // Vertical lines (parallel to Z axis)
  for (float x = -_Extent; x <= _Extent; x += _Step)
  {
    Vertices.push_back(x);
    Vertices.push_back(0.0f);
    Vertices.push_back(-_Extent);

    Vertices.push_back(x);
    Vertices.push_back(0.0f);
    Vertices.push_back(_Extent);
  }
  return Vertices;
}

} // namespace

CGridRenderPass::CGridRenderPass() :
    m_Shader(resource::LoadShader("Grid")),
    m_VAO(),
    m_VBO(GL_STATIC_DRAW),
    m_VertexCount(0)
{
  const std::vector<float> Grid = GenerateGridVertices(50.0f, 1.0f);

  m_VertexCount = static_cast<uint32_t>(Grid.size() / 3);

  m_VAO.Bind();
  m_VBO.Bind();
  m_VBO.Assign(Grid);
  m_VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
  m_VAO.Unbind();
}

void CGridRenderPass::PreExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthTest(true);
  _Renderer.SetDepthMask(false);
  _Renderer.SetDepthFunc(GL_LESS);
  _Renderer.SetBlending(EAlphaMode::Blend);
  _Renderer.SetCullFace(ECullMode::None);
  _Renderer.SetViewport(_RenderContext.PostProcessRenderTarget.Size);
  _Renderer.SetShader(m_Shader);
}

void CGridRenderPass::Execute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  m_VAO.Bind();

  glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
  Model           = glm::translate(Model, glm::vec3(0.0f, -0.01f, 0.0f)); // To prevent z-fighting

  _Renderer.SetUniform("u_MVP", _RenderContext.ViewProjectionMatrix * Model);
  _Renderer.SetUniform("u_Model", Model);

  _Renderer.DrawArrays(EPrimitiveMode::Lines, m_VertexCount);

  m_VAO.Unbind();
}

void CGridRenderPass::PostExecute(IRenderer &_Renderer, TRenderContext &_RenderContext, const IRenderPass::CommandsList &_Commands)
{
  _Renderer.SetDepthMask(true);
}

bool CGridRenderPass::Accepts(const TRenderCommand &_Command) const
{
  return false;
}

bool CGridRenderPass::IsAvailable() const
{
  return m_Shader != nullptr;
}

bool CGridRenderPass::NeedsCommands() const
{
  return false;
}
