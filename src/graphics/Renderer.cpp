#include "Renderer.h"
#include "Shader.h"
#include "engine/Camera.h"
#include <glm/gtc/type_ptr.hpp>

CRenderer::CRenderer() :
  m_LightingUBO(GL_STATIC_DRAW)
{
  m_LightingUBO.Bind();
  glBufferData(GL_UNIFORM_BUFFER, sizeof(TShaderLighting), NULL, GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_LightingUBO.ID());
  m_LightingUBO.Unbind();
}

void CRenderer::Clear(GLbitfield _Mask)
{
  glClear(_Mask);
}

void CRenderer::ClearColor(GLfloat _R, GLfloat _G, GLfloat _B, GLfloat _A)
{
  glClearColor(_R, _G, _B, _A);
}

void CRenderer::SetViewport(GLsizei _Width, GLsizei _Height)
{
  glViewport(0, 0, _Width, _Height);
}

void CRenderer::DrawArrays(GLenum _Mode, GLsizei _Count)
{
  glDrawArrays(_Mode, 0, _Count);
}

void CRenderer::SetCamera(const std::shared_ptr<CCamera> &_Camera)
{
  m_Camera = _Camera;
}

const std::shared_ptr<CCamera> &CRenderer::GetCamera() const
{
  return m_Camera;
}

void CRenderer::SetShader(const std::shared_ptr<CShader> & _Shader)
{
  m_CurrentShader = _Shader;

  InitShaderValues();
}

const std::shared_ptr<CShader> & CRenderer::GetShader() const
{
  return m_CurrentShader;
}

void CRenderer::SetLightingData(TShaderLighting && _Data)
{
  IS_SAME_TYPE(m_Lighting, _Data);
  memmove(&m_Lighting, &_Data, sizeof(m_Lighting));
}

void CRenderer::InitShaderValues()
{
  m_CurrentShader->Use();
  m_CurrentShader->Validate();
  m_CurrentShader->SetUniform("u_Projection", m_Camera->GetProjection());
  m_CurrentShader->SetUniform("u_ViewPos",    m_Camera->GetPosition());
  m_CurrentShader->SetUniform("u_View",       m_Camera->GetView());

  GLuint LightingDataLoc = glGetUniformBlockIndex(m_CurrentShader->GetID(), "u_Lighting");

  if (LightingDataLoc != GL_INVALID_INDEX)
  {
    m_LightingUBO.Bind();
    glUniformBlockBinding(m_CurrentShader->GetID(), LightingDataLoc, 0);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_Lighting), &m_Lighting);
    m_LightingUBO.Unbind();
  }
}
