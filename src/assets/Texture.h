#pragma once

#include "interfaces/GeneratableTexture.h"
#include "interfaces/TextureAsset.h"
#include <common/MathTypes.h>

class CTexture : public ITextureAsset,
                 public IGeneratableTexture
{
public:
  ~CTexture() override;
  void Shutdown() override;

  unsigned ID() const;
  bool IsValid() const;

  TVector2i GetSize() const;

  void Bind(unsigned _TextureUnit) const;
  void Unbind() const;

protected:
  CTexture(unsigned _Target);

  void OverrideTarget(unsigned _Target);

  static float GetSupportedAnisotropyLevel();

  static void Bind(unsigned _Target, unsigned _TextureUnit, unsigned _TextureID);
  static void Unbind(unsigned _Target);

public:
  static const unsigned INVALID_TEXTURE;

protected:
  unsigned  m_ID;
  unsigned  m_Target;
  TVector2i m_Size;
};

// ----------------------------------------------

class C2DTexture final : public CTexture
{
  static const unsigned int TARGET;
  static const unsigned int SAMPLED_TARGET;

public:
  C2DTexture();

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  static void Bind(unsigned _TextureUnit, unsigned _TextureID);
  static void Unbind();

private:
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params);
};

// ----------------------------------------------

class CCubemap final : public CTexture
{
  static const unsigned int TARGET;
  static const unsigned int CUBEMAP_FACES_COUNT;

public:
  CCubemap();

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) override;

  static void Bind(unsigned _TextureUnit, unsigned _TextureID);
  static void Unbind();

private:
  bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params);
  bool LoadLegacy(const std::filesystem::path &_Path, const TTextureParams &_Params);
};
