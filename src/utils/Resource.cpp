#include "Resource.h"
#include "engine/Engine.h"
#include "engine/ResourceManager.h"
#include "assets/TextureParams.h"
#include "pch.h"

static CResourceManager *Get()
{
  return CEngine::Instance().GetResourceManager().get();
}

namespace resource
{

std::shared_ptr<CModel> LoadModel(const std::filesystem::path &_Path)
{
  return Get()->LoadModel(_Path);
}

std::shared_ptr<CShader> LoadShader(const std::string &_Name)
{
  return Get()->LoadShader(_Name);
}

std::shared_ptr<CTexture> LoadTexture(const std::filesystem::path &_Path)
{
  return Get()->LoadTexture(_Path);
}

std::shared_ptr<CTexture> LoadTexture(const std::filesystem::path &_Path, const TTextureParams &_Params)
{
  return Get()->LoadTexture(_Path, _Params);
}

std::shared_ptr<CTexture> LoadCubemap(const std::filesystem::path &_Path)
{
  return Get()->LoadCubemap(_Path);
}

std::shared_ptr<CTexture> CreateCubemap(const std::string &_Name, const TTextureParams &_Params)
{
  return Get()->CreateCubemap(_Name, _Params);
}

std::shared_ptr<CTexture> CreateTexture(const std::string &_Name, const TTextureParams &_Params)
{
  return Get()->CreateTexture(_Name, _Params);
}

std::shared_ptr<CTexture> RecreateTexture(const std::string &_Name, const TTextureParams &_Params)
{
  Get()->Retire(_Name);
  return Get()->CreateTexture(_Name, _Params);
}

std::shared_ptr<CTexture> GetDefaultTexture(ETextureType _TextureType)
{
  return Get()->GetDefaultTexture(_TextureType);
}

void Retire(const std::string &_Name)
{
  Get()->Retire(_Name);
}

void Prune()
{
  Get()->Prune();
}

} // namespace resource
