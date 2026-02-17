#pragma once

#include <glad/glad.h>
#include "interfaces/GeneratableTexture.h"
#include "interfaces/TextureAsset.h"
#include "utils/StaticArray.h"

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

  static void Bind(GLenum _Target, GLenum _TextureUnit, GLuint _TextureID);
  static void Unbind(GLenum _Target);

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
  static constexpr inline GLenum FORMATS[]     = {GL_RED, GL_RED, GL_RGB, GL_RGBA};

  GLuint       m_ID;
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

  static constexpr inline GLenum TARGET = GL_TEXTURE_2D;

private:
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params);
};

class CCubemap final : public CTextureBase
{
  static constexpr inline int CUBEMAP_FACES_COUNT = 6;

  CStaticArray<std::string, CUBEMAP_FACES_COUNT> CUBEMAP_FACES = {"right", "left", "top", "bottom", "front", "back"};

public:
  CCubemap();

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  static constexpr inline GLenum TARGET = GL_TEXTURE_CUBE_MAP;

private:
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params);
};