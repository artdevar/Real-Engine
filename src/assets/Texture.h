#pragma once

#include <glad/glad.h>
#include "interfaces/GeneratableTexture.h"
#include "interfaces/TextureAsset.h"
#include <common/containers/StaticArray.h>
#include <common/MathTypes.h>

class CTextureBase : public ITextureAsset,
                     public IGeneratableTexture
{
  DISABLE_CLASS_COPY(CTextureBase);

public:
  ~CTextureBase() override;

  void Shutdown() override;

  void Bind(GLenum _TextureUnit);
  void Unbind();

  GLuint ID() const;
  bool IsValid() const;

  TVector2i GetSize() const;

  static constexpr inline GLuint INVALID_VALUE = 0u;

protected:
  CTextureBase(GLenum _Target);

  static void Bind(GLenum _Target, GLenum _TextureUnit, GLuint _TextureID);
  static void Unbind(GLenum _Target);

  struct TImage
  {
    void *Data;

    int Width;
    int Height;
    int Channels;
  };

protected:
  GLuint       m_ID;
  const GLenum m_Target;
  TVector2i    m_Size;
};

//

class CTexture final : public CTextureBase
{
public:
  CTexture();

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  static void Bind(GLenum _TextureUnit, GLuint _TextureID);
  static void Unbind();

  static constexpr inline GLenum TARGET = GL_TEXTURE_2D;

private:
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params);
};

class CCubemap final : public CTextureBase
{
  static constexpr inline int CUBEMAP_FACES_COUNT = 6;

public:
  CCubemap();

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  static void Bind(GLenum _TextureUnit, GLuint _TextureID);
  static void Unbind();

  static constexpr inline GLenum TARGET = GL_TEXTURE_CUBE_MAP;

private:
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params);
  bool LoadLegacy(const std::filesystem::path &_Path, const TTextureParams &_Params);
};