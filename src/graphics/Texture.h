#pragma once

#include <glad/glad.h>
#include "interfaces/TextureAsset.h"
#include "interfaces/GeneratableTexture.h"

class CTextureBase : public ITextureAsset,
                     public IGeneratableTexture
{
  DISABLE_CLASS_COPY(CTextureBase);

public:
  ~CTextureBase() override;

  void Shutdown() override;

  void Bind(GLenum _TextureUnit);
  void Unbind();

  GLuint Get() const;

  bool IsValid() const;

protected:
  struct TImage
  {
    unsigned char *Data;

    int Width;
    int Height;
    int Channels;
  };

  CTextureBase(GLenum _Target);

  static constexpr inline GLuint INVALID_VALUE = 0u;

  GLuint m_ID;
  const GLenum m_Target;
};

//

class CTexture final : public CTextureBase
{
public:
  CTexture();

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override;

private:
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params);
};

class CCubemap final : public CTextureBase
{
  static constexpr inline int CUBEMAP_FACES = 6;

public:
  CCubemap();

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override;
};