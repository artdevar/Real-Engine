#include "Renderer.h"
#include "Shader.h"
#include "engine/Camera.h"
#include "engine/Config.h"
#include "utils/Logger.h"
#include <cstring>
#include <glm/gtc/type_ptr.hpp>

CRenderer::CRenderer() : m_LightingUBO(GL_DYNAMIC_DRAW)
{
  m_LightingUBO.Bind();
  m_LightingUBO.Reserve(sizeof(TShaderLighting));
  m_LightingUBO.BindToBase(BINDING_LIGHTING_BUFFER);
  m_LightingUBO.Unbind();

  std::memset(&m_Lighting, 0, sizeof(m_Lighting));
}

void CRenderer::BeginFrame(float _R, float _G, float _B, float _A, ClearFlags _ClearFlags)
{
  glClearColor(_R, _G, _B, _A);
  glClear(static_cast<GLbitfield>(_ClearFlags));
}

void CRenderer::EndFrame()
{
  CheckErrors();
}

void CRenderer::CheckErrors()
{
  while (true)
  {
    GLenum Error = glGetError();
    if (Error != GL_NO_ERROR)
      CLogger::Log(ELogType::Error, "[OpenGL] Error: {}", GetGLErrorDescription(Error));
    else
      break;
  }
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

glm::ivec2 CRenderer::GetViewport() const
{
  GLint Viewport[4] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, Viewport);
  return glm::ivec2(Viewport[2], Viewport[3]);
}

void CRenderer::DrawArrays(GLenum _Mode, GLsizei _Count)
{
  glDrawArrays(_Mode, 0, _Count);
}

void CRenderer::DrawElements(GLenum _Mode, GLsizei _Count, GLenum _Type, const void *_Offset)
{
  glDrawElements(_Mode, _Count, _Type, _Offset);
}

void CRenderer::SetCamera(const std::shared_ptr<CCamera> &_Camera)
{
  m_Camera = _Camera;
}

const std::shared_ptr<CCamera> &CRenderer::GetCamera() const
{
  return m_Camera;
}

void CRenderer::SetShader(const std::shared_ptr<CShader> &_Shader)
{
  m_CurrentShader = _Shader;

  InitShaderValues();
}

const std::shared_ptr<CShader> &CRenderer::GetShader() const
{
  return m_CurrentShader;
}

void CRenderer::SetLightingData(TShaderLighting &&_Data)
{
  IS_SAME_TYPE(m_Lighting, _Data);
  std::memcpy(&m_Lighting, &_Data, sizeof(m_Lighting));
  m_LightSpaceMatrixDirty = true;
}

glm::mat4 CRenderer::GetLightSpaceMatrix() const
{
  if (m_LightSpaceMatrixDirty)
  {
    const float NearPlane = CConfig::Instance().GetLightSpaceMatrixZNear();
    const float FarPlane = CConfig::Instance().GetLightSpaceMatrixZFar();
    const float LeftBot = CConfig::Instance().GetLightSpaceMatrixOrthLeftBot();
    const float RightTop = CConfig::Instance().GetLightSpaceMatrixOrthRightTop();

    const glm::vec3 LightDir = m_Lighting.LightDirectional.Direction * -1.0f;
    const glm::mat4 LightProjection = glm::ortho(LeftBot, RightTop, LeftBot, RightTop, NearPlane, FarPlane);
    const glm::mat4 LightView = glm::lookAt(LightDir, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

    m_LightSpaceMatrix = LightProjection * LightView;
    m_LightSpaceMatrixDirty = false;
  }

  return m_LightSpaceMatrix;
}

void CRenderer::SetBlending(bool _Enabled)
{
  if (_Enabled)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  else
  {
    glDisable(GL_BLEND);
  }
}

void CRenderer::SetCullFace(GLenum _Mode)
{
  if (m_CullingMode == _Mode)
    return;

  m_CullingMode = _Mode;

  switch (_Mode)
  {
  case GL_FRONT:
  case GL_BACK:
  case GL_FRONT_AND_BACK:
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(_Mode);
    break;

  case GL_NONE:
    glDisable(GL_CULL_FACE);
    break;

  default:
    assert(false && "Invalid culling mode");
    break;
  }
}

void CRenderer::SetUniform(std::string_view _Name, const UniformType &_Value)
{
  if (!m_CurrentShader)
    return;

  m_CurrentShader->SetUniform(_Name, _Value);
}

void CRenderer::InitShaderValues()
{
  m_CurrentShader->Use();
  m_CurrentShader->Validate();

  GLuint LightingDataLoc = glGetUniformBlockIndex(m_CurrentShader->GetID(), "u_Lighting");
  if (LightingDataLoc != GL_INVALID_INDEX)
  {
    m_LightingUBO.Bind();
    m_LightingUBO.BindToUniformBlock(m_CurrentShader->GetID(), LightingDataLoc, BINDING_LIGHTING_BUFFER);
    m_LightingUBO.Assign(&m_Lighting, sizeof(m_Lighting));
    m_LightingUBO.Unbind();
  }
}

std::string CRenderer::GetGLErrorDescription(GLenum _Error)
{
  switch (_Error)
  {
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";

  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";

  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";

  case GL_STACK_OVERFLOW:
    return "GL_STACK_OVERFLOW";

  case GL_STACK_UNDERFLOW:
    return "GL_STACK_UNDERFLOW";

  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";

  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";

  default:
    return "UNDEFINED";
  }
}