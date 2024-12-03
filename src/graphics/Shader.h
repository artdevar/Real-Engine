#pragma once

#include "interfaces/Shutdownable.h"
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <string>
#include <string_view>
#include <variant>
#include <map>

class CShader final :
  public IShutdownable
{
  using UniformType = std::variant<GLint, GLuint, GLfloat, glm::mat3, glm::mat4, glm::vec2, glm::vec3, glm::vec4>;

public:

  CShader();

  CShader(const CShader &) = delete;

  void Shutdown() override;

  bool Init(const std::string & _Path);

  GLuint GetID() const;

  void Use() const;

  bool IsValid() const;

  void SetUniform(const std::string_view & _Name, const UniformType & _Value);

  void Validate();

private:

  bool IsUsed() const;

  GLuint LoadShader(const std::string & _Path, GLenum _ShaderType);

private:

  GLuint m_ID;
  std::map<std::string, GLuint> m_UniformsCache;

};