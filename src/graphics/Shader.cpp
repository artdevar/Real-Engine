#include "Shader.h"
#include "utils/Logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

static inline bool operator<(const std::string &_L, const std::string_view &_R)
{
  return _L < _R;
}

template <class>
static constexpr bool AlwaysFalse = false;

CShader::CShader() : m_ID(INVALID_VALUE)
{
}

CShader::~CShader()
{
  assert(!IsValid() && "The shader isn't completely shutted down");
}

void CShader::Shutdown()
{
  if (!IsValid())
    return;

  constexpr GLsizei MaxCount = 10;

  GLsizei Count = 0;
  GLuint Shaders[MaxCount] = {0};

  glGetAttachedShaders(m_ID, MaxCount, &Count, Shaders);

  for (int i = 0; i < Count; ++i)
    glDeleteShader(Shaders[i]);

  glDeleteProgram(m_ID);
  m_ID = INVALID_VALUE;
}

bool CShader::Load(const std::filesystem::path &_Path)
{
  const std::filesystem::path VertexShaderPath = std::filesystem::path(_Path).replace_extension(".vs");
  const GLuint VertexShader = LoadShader(VertexShaderPath, GL_VERTEX_SHADER);
  if (VertexShader == INVALID_VALUE)
    return false;

  const std::filesystem::path FragmentShaderPath = std::filesystem::path(_Path).replace_extension(".fs");
  const GLuint FragmentShader = LoadShader(FragmentShaderPath, GL_FRAGMENT_SHADER);
  if (FragmentShader == INVALID_VALUE)
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
  if (Success == GL_FALSE)
  {
    char ErrorLog[512] = {'\0'};
    glGetShaderInfoLog(m_ID, 512, NULL, ErrorLog);
    CLogger::Log(ELogType::Error, std::format("Shader '{}' linkage error:\n{}", _Path.c_str(), ErrorLog));
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
  return m_ID != INVALID_VALUE;
}

void CShader::SetUniform(const std::string_view &_Name, const UniformType &_Value)
{
  assert(IsValid());
  assert(IsUsed());

  auto UniformLocIter = m_UniformsCache.find(_Name);
  if (UniformLocIter == m_UniformsCache.end())
    UniformLocIter = m_UniformsCache.emplace(_Name.data(), glGetUniformLocation(GetID(), _Name.data())).first;

  std::visit([UniformLoc = UniformLocIter->second](auto &&_Arg)
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
      static_assert(AlwaysFalse<Type>, "Non-exhaustive visitor"); }, _Value);
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

GLuint CShader::LoadShader(const std::filesystem::path &_Path, GLenum _ShaderType)
{
  std::ifstream ShaderFile(_Path);
  if (!ShaderFile.is_open())
  {
    CLogger::Log(ELogType::Error, std::format("Shader '{}' is absent", _Path.c_str()));
    return INVALID_VALUE;
  }

  std::stringstream FileStream;
  FileStream << ShaderFile.rdbuf();
  std::string FileContent = FileStream.str();
  ShaderFile.close();

  GLuint Shader = glCreateShader(_ShaderType);
  assert(Shader != INVALID_VALUE && "Shader is not created");

  const GLchar *ShaderCode = FileContent.c_str();
  glShaderSource(Shader, 1, &ShaderCode, nullptr);
  glCompileShader(Shader);

  GLint Success;
  glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
  if (!Success)
  {
    char ErrorLog[512] = {'\0'};
    glGetShaderInfoLog(Shader, 512, NULL, ErrorLog);
    CLogger::Log(ELogType::Error, std::format("Shader '{}' compilation error:\n{}", _Path.c_str(), ErrorLog));
    glDeleteShader(Shader);
    Shader = INVALID_VALUE;
  }

  return Shader;
}
