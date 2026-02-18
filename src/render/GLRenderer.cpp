#include "GLRenderer.h"
#include "assets/Shader.h"
#include "engine/Camera.h"
#include "engine/Config.h"
#include "assets/Texture.h"
#include "utils/Logger.h"

void COpenGLRenderer::BeginFrame(const TColor &_ClearColor, EClearFlags _ClearFlags)
{
  m_CurrentShader.reset();
  ClearColor(_ClearColor);
  Clear(_ClearFlags);
}

void COpenGLRenderer::EndFrame()
{
  CheckErrors();
}

void COpenGLRenderer::CheckErrors()
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

void COpenGLRenderer::Clear(EClearFlags _ClearFlags)
{
  GLbitfield Mask = 0;
  if (_ClearFlags & EClearFlags::Color)
    Mask |= GL_COLOR_BUFFER_BIT;
  if (_ClearFlags & EClearFlags::Depth)
    Mask |= GL_DEPTH_BUFFER_BIT;
  if (_ClearFlags & EClearFlags::Stencil)
    Mask |= GL_STENCIL_BUFFER_BIT;
  glClear(Mask);
}

void COpenGLRenderer::ClearColor(const TColor &_Color)
{
  glClearColor(_Color.R, _Color.G, _Color.B, _Color.A);
}

void COpenGLRenderer::SetViewport(TVector2i _Viewport)
{
  glViewport(0, 0, _Viewport.X, _Viewport.Y);
}

TVector2i COpenGLRenderer::GetViewport() const
{
  GLint Viewport[4] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, Viewport);
  return TVector2i(Viewport[2], Viewport[3]);
}

void COpenGLRenderer::DrawArrays(EPrimitiveMode mode, int count)
{
  GLenum glMode = GL_TRIANGLES;
  switch (mode)
  {
  case EPrimitiveMode::Points:
    glMode = GL_POINTS;
    break;
  case EPrimitiveMode::Line:
    glMode = GL_LINES;
    break;
  case EPrimitiveMode::LineLoop:
    glMode = GL_LINE_LOOP;
    break;
  case EPrimitiveMode::LineStrip:
    glMode = GL_LINE_STRIP;
    break;
  case EPrimitiveMode::Triangles:
    glMode = GL_TRIANGLES;
    break;
  case EPrimitiveMode::TriangleStrip:
    glMode = GL_TRIANGLE_STRIP;
    break;
  case EPrimitiveMode::TriangleFan:
    glMode = GL_TRIANGLE_FAN;
    break;
  default:
    assert(false && "Invalid primitive mode");
    break;
  }
  glDrawArrays(glMode, static_cast<GLint>(0), static_cast<GLsizei>(count));
}

void COpenGLRenderer::DrawElements(EPrimitiveMode mode, int count, EIndexType indexType, const void *offset)
{
  GLenum glMode = GL_TRIANGLES;
  switch (mode)
  {
  case EPrimitiveMode::Points:
    glMode = GL_POINTS;
    break;
  case EPrimitiveMode::Line:
    glMode = GL_LINES;
    break;
  case EPrimitiveMode::LineLoop:
    glMode = GL_LINE_LOOP;
    break;
  case EPrimitiveMode::LineStrip:
    glMode = GL_LINE_STRIP;
    break;
  case EPrimitiveMode::Triangles:
    glMode = GL_TRIANGLES;
    break;
  case EPrimitiveMode::TriangleStrip:
    glMode = GL_TRIANGLE_STRIP;
    break;
  case EPrimitiveMode::TriangleFan:
    glMode = GL_TRIANGLE_FAN;
    break;
  default:
    assert(false && "Invalid primitive mode");
    break;
  }

  GLenum glIndexType = GL_UNSIGNED_INT;
  switch (indexType)
  {
  case EIndexType::Byte:
    glIndexType = GL_BYTE;
    break;
  case EIndexType::UnsignedByte:
    glIndexType = GL_UNSIGNED_BYTE;
    break;
  case EIndexType::Short:
    glIndexType = GL_SHORT;
    break;
  case EIndexType::UnsignedShort:
    glIndexType = GL_UNSIGNED_SHORT;
    break;
  case EIndexType::Int:
    glIndexType = GL_INT;
    break;
  case EIndexType::UnsignedInt:
    glIndexType = GL_UNSIGNED_INT;
    break;
  case EIndexType::Float:
    glIndexType = GL_FLOAT;
    break;
  default:
    assert(false && "Invalid index type");
    break;
  }

  glDrawElements(glMode, static_cast<GLsizei>(count), static_cast<GLenum>(glIndexType), offset);
}

void COpenGLRenderer::SetCamera(const std::shared_ptr<CCamera> &_Camera)
{
  m_Camera = _Camera;
}

const std::shared_ptr<CCamera> &COpenGLRenderer::GetCamera() const
{
  return m_Camera;
}

void COpenGLRenderer::SetShader(const std::shared_ptr<CShader> &_Shader)
{
  if (m_CurrentShader == _Shader)
    return;

  m_CurrentShader = _Shader;
  m_CurrentShader->Use();
  m_CurrentShader->Validate();
}

const std::shared_ptr<CShader> &COpenGLRenderer::GetShader() const
{
  return m_CurrentShader;
}

void COpenGLRenderer::SetBlending(EAlphaMode _Mode)
{
  if (m_AlphaMode == _Mode)
    return;

  m_AlphaMode = _Mode;

  switch (_Mode)
  {
  case EAlphaMode::Opaque:
    glDisable(GL_BLEND);
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    break;

  case EAlphaMode::Mask:
    glDisable(GL_BLEND);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE); // optional
    break;

  case EAlphaMode::Blend:
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  }
}

void COpenGLRenderer::SetDepthTest(bool _Enable)
{
  if (_Enable)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}

void COpenGLRenderer::SetDepthFunc(int _Func)
{
  glDepthFunc(_Func);
}

void COpenGLRenderer::SetDepthMask(bool _Flag)
{
  glDepthMask(_Flag ? GL_TRUE : GL_FALSE);
}

void COpenGLRenderer::SetCullFace(ECullMode _Mode)
{
  if (m_CullingMode == _Mode)
    return;

  m_CullingMode = _Mode;

  const GLenum glMode = _Mode == ECullMode::Front ? GL_FRONT : _Mode == ECullMode::Back ? GL_BACK : GL_FRONT_AND_BACK;

  switch (_Mode)
  {
  case ECullMode::Front:
    [[fallthrough]];
  case ECullMode::Back:
    [[fallthrough]];
  case ECullMode::FrontAndBack:
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(glMode);
    break;

  case ECullMode::None:
    glDisable(GL_CULL_FACE);
    break;

  default:
    assert(false && "Invalid culling mode");
    break;
  }
}

void COpenGLRenderer::SetUniform(std::string_view _Name, const UniformType &_Value)
{
  if (!m_CurrentShader)
    return;

  m_CurrentShader->SetUniform(_Name, _Value);
}

std::string COpenGLRenderer::GetGLErrorDescription(GLenum _Error)
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