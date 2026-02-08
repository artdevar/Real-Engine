#pragma once

#include "interfaces/Shutdownable.h"
#include "utils/Common.h"
#include "graphics/TextureParams.h"
#include <string>

class CModel;
class CShader;
class CTextureBase;
class IAsset;

class CResourceManager final : public IShutdownable
{
  DISABLE_CLASS_COPY(CResourceManager);

public:
  CResourceManager() = default;

  void Init();

  void Shutdown() override;

  std::shared_ptr<CModel> LoadModel(const std::filesystem::path &_Path);

  std::shared_ptr<CShader> LoadShader(const std::filesystem::path &_Path);

  std::shared_ptr<CTextureBase> GetFallbackTexture() const;
  std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path &_Path);
  std::shared_ptr<CTextureBase> LoadCubemap(const std::filesystem::path &_Path);
  std::shared_ptr<CTextureBase> CreateTexture(const std::string &_Name,
                                              const TTextureParams &_Params = {});

private:
  static const std::filesystem::path DEFAULT_TEXTURE_PATH;

  std::map<std::string, std::shared_ptr<IAsset>, std::less<>> m_Assets;
};
