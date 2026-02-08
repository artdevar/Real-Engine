#pragma once

#include <glad/glad.h>
#include <glm/fwd.hpp>
#include "ShaderTypes.h"
#include "interfaces/Asset.h"

class CShader final : public IAsset
{
public:
  CShader();

  CShader(const CShader &) = delete;

  ~CShader() override;

  void Shutdown() override;

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;

  GLuint GetID() const;

  void Use() const;

  bool IsValid() const;

  void SetUniform(std::string_view _Name, const UniformType &_Value);

  void Validate();

private:
  bool IsUsed() const;

  static GLuint LoadShader(const std::filesystem::path &_Path, GLenum _ShaderType);
  static void UnloadShader(GLuint _ShaderID);

private:
  static constexpr inline GLuint INVALID_VALUE = 0u;

  GLuint m_ID;
  std::map<std::string, GLuint, std::less<>> m_UniformsCache;

#if SHADERS_HOT_RELOAD
  std::filesystem::path m_BasePath;
  std::filesystem::file_time_type m_VertexTimestamp{};
  std::filesystem::file_time_type m_FragmentTimestamp{};
#endif
};