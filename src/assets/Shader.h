#pragma once

#include "render/ShaderTypes.h"
#include "interfaces/Asset.h"

class CShader final : public IAsset
{
public:
  CShader();

  CShader(const CShader &) = delete;

  ~CShader() override;

  void Shutdown() override;
  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;

  unsigned GetID() const;
  void Use() const;
  bool IsValid() const;

  void SetUniform(std::string_view _Name, const UniformType &_Value);
  void SetUniformBlockBinding(std::string_view _BlockName, unsigned _UniformBlockBinding);

  void Validate();

private:
  bool IsUsed() const;

  static unsigned LoadShader(const std::filesystem::path &_Path, unsigned _ShaderType);
  static void UnloadShader(unsigned _ShaderID);

private:
  static constexpr inline unsigned INVALID_VALUE = 0u;

  unsigned                                     m_ID;
  std::map<std::string, unsigned, std::less<>> m_UniformsCache;

#if SHADERS_HOT_RELOAD
  std::filesystem::path           m_BasePath;
  std::filesystem::file_time_type m_VertexTimestamp{};
  std::filesystem::file_time_type m_FragmentTimestamp{};
#endif
};