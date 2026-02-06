#pragma once

#include "interfaces/Shutdownable.h"
#include "utils/Common.h"
#include <filesystem>
#include <memory>
#include <string>
#include <map>

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

  std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path &_Path);

  std::shared_ptr<CTextureBase> LoadCubemap(const std::filesystem::path &_Path);

private:
  static const std::filesystem::path DEFAULT_TEXTURE_PATH;

  std::map<std::string, std::shared_ptr<IAsset>, std::less<>> m_Assets;
};
