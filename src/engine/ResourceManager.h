#pragma once

#include "assets/TextureParams.h"
#include <events/EventsListener.h>
#include <common/interfaces/Shutdownable.h>
#include <common/Sharable.h>
#include <common/interfaces/Updateable.h>

class CModel;
class CShader;
class CTexture;
class IAsset;

class CResourceManager final : public CSharable<CResourceManager>,
                               public IEventsListener,
                               public IUpdateable,
                               public IShutdownable
{
public:
  CResourceManager();

  void Init();

  void Shutdown() override;
  void Update(float _TimeDelta) override;

  void OnEvent(const TEvent &_Event) override;

  std::shared_ptr<CShader> LoadShader(const std::string &_Name);
  std::shared_ptr<CModel> LoadModel(const std::filesystem::path &_Path);

  std::shared_ptr<CTexture> GetDefaultTexture(ETextureType _TextureType);
  std::shared_ptr<CTexture> LoadTexture(const std::filesystem::path &_Path);
  std::shared_ptr<CTexture> LoadTexture(const std::filesystem::path &_Path, const TTextureParams &_Params);
  std::shared_ptr<CTexture> LoadCubemap(const std::filesystem::path &_Path);
  std::shared_ptr<CTexture> CreateCubemap(const std::string &_Name, const TTextureParams &_Params);
  std::shared_ptr<CTexture> CreateTexture(const std::string &_Name, const TTextureParams &_Params);

  void Retire(const std::string &_Name);
  void Prune();

private:
  void UnloadUnusedAssets();

private:
  static std::filesystem::path GetDefaultTexturePath(ETextureType _TextureType);

private:
  std::map<std::string, std::shared_ptr<IAsset>> m_Assets;
  bool                                           m_IsPruneScheduled;
};
