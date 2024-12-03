#include "Shader.h"
#include "utils/Logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

template <class>
static constexpr bool AlwaysFalse = false;

CShader::CShader() : m_ID(0)
{}

void CShader::Shutdown()
{
  if (!IsValid())
    return;

  constexpr GLsizei MaxCount = 10;

  GLsizei Count = 0;
  GLuint  Shaders[MaxCount] = { 0 };

  glGetAttachedShaders(m_ID, MaxCount, &Count, Shaders);

  for (int i = 0; i < Count; ++i)
    glDeleteShader(Shaders[i]);

  glDeleteProgram(m_ID);
  m_ID = 0;
}

bool CShader::Init(const std::string & _Path)
{
  const GLuint VertexShader = LoadShader(_Path + ".vert", GL_VERTEX_SHADER);
  if (VertexShader == 0)
    return false;

  const GLuint FragmentShader = LoadShader(_Path + ".frag", GL_FRAGMENT_SHADER);
  if (FragmentShader == 0)
  {
    glDeleteShader(VertexShader);
    return false;
  }

  m_ID = glCreateProgram();
  glAttachShader(m_ID, VertexShader);
  glAttachShader(m_ID, FragmentShader);
  glLinkProgram(m_ID);

  glDeleteShader(VertexShader);
  glDeleteShader(FragmentShader);

  GLint Success;
  glGetProgramiv(m_ID, GL_LINK_STATUS, &Success);
  if (!Success)
  {
    char ErrorLog[512] = {'\0'};
    glGetShaderInfoLog(m_ID, 512, NULL, ErrorLog);
    CLogger::Log(ELogType::Error, std::format("Shader '{}' linkage error:\n{}\n", _Path, ErrorLog));
    Shutdown();
    return false;
  }

  return true;
}

GLuint CShader::GetID() const
{
  return m_ID;
}

void CShader::Use() const
{
  assert(IsValid());
  glUseProgram(m_ID);
}

bool CShader::IsValid() const
{
  return m_ID != 0;
}

void CShader::SetUniform(const std::string_view & _Name, const UniformType & _Value)
{
  assert(IsValid());
  assert(IsUsed());

  auto UniformLocIter = m_UniformsCache.find(_Name.data());
  if (UniformLocIter == m_UniformsCache.end())
    UniformLocIter = m_UniformsCache.emplace(_Name.data(), glGetUniformLocation(GetID(), _Name.data())).first;

  const GLint UniformLoc = UniformLocIter->second;

  std::visit([UniformLoc](auto && _Arg)
  {
    using Type = std::decay_t<decltype(_Arg)>;
    if constexpr (std::is_same_v<Type, GLint>)
      glUniform1i(UniformLoc, _Arg);
    else if constexpr (std::is_same_v<Type, GLuint>)
      glUniform1ui(UniformLoc, _Arg);
    else if constexpr (std::is_same_v<Type, GLfloat>)
      glUniform1f(UniformLoc, _Arg);
    else if constexpr (std::is_same_v<Type, glm::mat3>)
      glUniformMatrix3fv(UniformLoc, 1, GL_FALSE, glm::value_ptr(_Arg));
    else if constexpr (std::is_same_v<Type, glm::mat4>)
      glUniformMatrix4fv(UniformLoc, 1, GL_FALSE, glm::value_ptr(_Arg));
    else if constexpr (std::is_same_v<Type, glm::vec2>)
      glUniform2fv(UniformLoc, 1, glm::value_ptr(_Arg));
    else if constexpr (std::is_same_v<Type, glm::vec3>)
      glUniform3fv(UniformLoc, 1, glm::value_ptr(_Arg));
    else if constexpr (std::is_same_v<Type, glm::vec4>)
      glUniform4fv(UniformLoc, 1, glm::value_ptr(_Arg));
    else
      static_assert(AlwaysFalse<Type>, "Non-exhaustive visitor");
  }, _Value);
}

void CShader::Validate()
{
#ifdef SHADER_HOT_RELOAD

#endif
}

bool CShader::IsUsed() const
{
  GLint Program;
  glGetIntegerv(GL_CURRENT_PROGRAM, &Program);
  return GLuint(Program) == GetID();
}

GLuint CShader::LoadShader(const std::string &_Path, GLenum _ShaderType)
{
  std::ifstream ShaderFile(_Path);
  if (!ShaderFile.is_open())
  {
    CLogger::Log(ELogType::Error, std::format("Shader '{}' is absent\n", _Path));
    return 0;
  }

  std::stringstream FileStream;
  FileStream << ShaderFile.rdbuf();
  std::string FileContent = FileStream.str();
  ShaderFile.close();

  GLuint Shader = glCreateShader(_ShaderType);
  assert(Shader && "Shader is not created");

  const GLchar * ShaderCode = FileContent.c_str();
  glShaderSource(Shader, 1, &ShaderCode, nullptr);
  glCompileShader(Shader);

  GLint Success;
  glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
  if (!Success)
  {
    char ErrorLog[512] = {'\0'};
    glGetShaderInfoLog(Shader, 512, NULL, ErrorLog);
    CLogger::Log(ELogType::Error, std::format("Shader '{}' compilation error:\n{}\n", _Path, ErrorLog));
    glDeleteShader(Shader);
    Shader = 0;
  }

  return Shader;
}
