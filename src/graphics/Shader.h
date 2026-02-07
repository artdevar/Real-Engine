#pragma once

#include <glad/glad.h>
#include <glm/fwd.hpp>
#include "ShaderTypes.h"
#include "interfaces/Asset.h"
#include <string>
#include <string_view>
#include <map>

class CShader final : public IAsset
{
public:
  CShader();

  CShader(const CShader &) = delete;

  ~CShader() override;

  void Shutdown() override;

  bool Load(const std::filesystem::path &_Path, CResourceManagerKey) override;

  GLuint GetID() const;

  void Use() const;

  bool IsValid() const;

  void SetUniform(std::string_view _Name, const UniformType &_Value);

  void Validate();

private:
  bool IsUsed() const;

  GLuint LoadShader(const std::filesystem::path &_Path, GLenum _ShaderType);

private:
  static constexpr inline GLuint INVALID_VALUE = 0u;

  GLuint m_ID;
  std::map<std::string, GLuint, std::less<>> m_UniformsCache;
};