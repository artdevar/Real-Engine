#pragma once

#include "interfaces/Shutdownable.h"
#include "utils/Common.h"

class CModel;
class CShader;
class CTextureBase;
class IAsset;
struct TTextureParams;

class CResourceManager final : public IShutdownable
{
  DISABLE_CLASS_COPY(CResourceManager);

public:
  CResourceManager() = default;

  void Init();

  void Shutdown() override;

  std::shared_ptr<CShader> LoadShader(const std::string &_Name);
  std::shared_ptr<CModel> LoadModel(const std::filesystem::path &_Path);

  std::shared_ptr<CTextureBase> GetDefaultBasicTexture();
  std::shared_ptr<CTextureBase> GetDefaultNormalTexture();
  std::shared_ptr<CTextureBase> GetDefaultRoughnessTexture();
  std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path &_Path);
  std::shared_ptr<CTextureBase> LoadCubemap(const std::filesystem::path &_Path);
  std::shared_ptr<CTextureBase> CreateTexture(const std::string &_Name,
                                              const TTextureParams &_Params);

private:
  static std::filesystem::path GetDefaultBasicTexturePath();
  static std::filesystem::path GetDefaultNormalTexturePath();
  static std::filesystem::path GetDefaultRoughnessTexturePath();

private:
  std::map<std::string, std::shared_ptr<IAsset>, std::less<>> m_Assets;
};
