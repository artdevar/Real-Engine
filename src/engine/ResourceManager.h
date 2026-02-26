#pragma once

#include "assets/TextureParams.h"
#include "interfaces/Shutdownable.h"
#include "interfaces/EventsListener.h"
#include "interfaces/Sharable.h"
#include "utils/Common.h"

class CModel;
class CShader;
class CTextureBase;
class IAsset;

class CResourceManager final : public CSharable<CResourceManager>,
                               public IEventsListener,
                               public IShutdownable
{
  DISABLE_CLASS_COPY(CResourceManager);

public:
  CResourceManager() = default;

  void Init();

  void Shutdown() override;

  void OnEvent(const TEvent &_Event) override;

  std::shared_ptr<CShader> LoadShader(const std::string &_Name);
  std::shared_ptr<CModel> LoadModel(const std::filesystem::path &_Path);

  std::shared_ptr<CTextureBase> GetDefaultTexture(ETextureType _TextureType);
  std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path &_Path);
  std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path &_Path, const TTextureParams &_Params);
  std::shared_ptr<CTextureBase> LoadCubemap(const std::filesystem::path &_Path);
  std::shared_ptr<CTextureBase> CreateTexture(const std::string &_Name, const TTextureParams &_Params);

  void MarkUnused(const std::string &_Name);

private:
  void RemoveUnusedAssets();

  static std::filesystem::path GetDefaultTexturePath(ETextureType _TextureType);

private:
  std::map<std::string, std::shared_ptr<IAsset>, std::less<>> m_Assets;
};
