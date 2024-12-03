#pragma once

#include "interfaces/Shutdownable.h"
#include <filesystem>
#include <memory>
#include <string>
#include <map>

struct TModel;
class CShader;
class CTextureBase;

class CResourceManager final :
  public IShutdownable
{
public:

  CResourceManager() = default;

  CResourceManager(const CResourceManager &) = delete;

  void Shutdown() override;

  std::shared_ptr<TModel> LoadModel(const std::filesystem::path & _Path);

  std::shared_ptr<CShader> LoadShader(const std::string & _Path);

  std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path & _Path);

  std::shared_ptr<CTextureBase> LoadCubemap(const std::filesystem::path & _Path);

private:

  std::map<std::string, std::shared_ptr<TModel>>       m_Models;
  std::map<std::string, std::shared_ptr<CShader>>      m_Shaders;
  std::map<std::string, std::shared_ptr<CTextureBase>> m_Textures;

};