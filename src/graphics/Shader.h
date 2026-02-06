#pragma once

#include "interfaces/Asset.h"
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <string>
#include <string_view>
#include <variant>
#include <map>

class CShader final : public IAsset
{
  using UniformType = std::variant<GLint, GLuint, GLfloat, glm::mat3, glm::mat4, glm::vec2, glm::vec3, glm::vec4>;

public:
  CShader();

  CShader(const CShader &) = delete;

  ~CShader() override;

  void Shutdown() override;

  bool Load(const std::filesystem::path &_Path, CResourceManagerKey) override;

  GLuint GetID() const;

  void Use() const;

  bool IsValid() const;

  void SetUniform(const std::string_view &_Name, const UniformType &_Value);

  void Validate();

private:
  bool IsUsed() const;

  GLuint LoadShader(const std::filesystem::path &_Path, GLenum _ShaderType);

private:
  static constexpr inline GLuint INVALID_VALUE = 0u;

  GLuint m_ID;
  std::map<std::string, GLuint, std::less<>> m_UniformsCache;
};