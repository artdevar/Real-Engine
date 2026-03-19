#pragma once

#include "interfaces/GeneratableTexture.h"
#include "interfaces/TextureAsset.h"
#include <common/MathTypes.h>

class CTextureBase : public ITextureAsset,
                     public IGeneratableTexture
{
  DISABLE_CLASS_COPY(CTextureBase);

public:
  ~CTextureBase() override;

  void Shutdown() override;

  void Bind(unsigned _TextureUnit);
  void Unbind();

  unsigned ID() const;
  bool IsValid() const;

  TVector2i GetSize() const;

  static constexpr inline unsigned INVALID_VALUE = 0u;

protected:
  CTextureBase(unsigned _Target);

  static void Bind(unsigned _Target, unsigned _TextureUnit, unsigned _TextureID);
  static void Unbind(unsigned _Target);

protected:
  unsigned  m_ID;
  unsigned  m_Target;
  TVector2i m_Size;
};

// ----------------------------------------------

class CTexture final : public CTextureBase
{
public:
  CTexture();

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  static void Bind(unsigned _TextureUnit, unsigned _TextureID);
  static void Unbind();

  static const unsigned TARGET;

private:
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params);

  static float GetSupportedAnisotropyLevel();
};

// ----------------------------------------------

class CCubemap final : public CTextureBase
{
  static constexpr inline int CUBEMAP_FACES_COUNT = 6;

public:
  CCubemap();

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  static void Bind(unsigned _TextureUnit, unsigned _TextureID);
  static void Unbind();

  static const unsigned TARGET;

private:
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params);
  bool LoadLegacy(const std::filesystem::path &_Path, const TTextureParams &_Params);
};
