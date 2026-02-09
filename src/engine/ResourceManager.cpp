#include "pch.h"

#include "ResourceManager.h"
#include "engine/Config.h"
#include "Passkey.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"
#include "graphics/TextureParams.h"
#include "graphics/TinyGLTFParseStrategy.h"
#include "utils/Logger.h"

static inline bool operator<(const std::string &_L, const std::filesystem::path &_R)
{
  return _L < _R.c_str();
}

static bool IsFormatSupported(const std::filesystem::path &_Path)
{
  if (_Path.extension() == ".gltf")
    return true;

  CLogger::Log(ELogType::Error, "Unsupported model format '{}'", _Path.string());
  return false;
}

void CResourceManager::Init()
{
  LoadTexture(GetDefaultTexturePath());
}

void CResourceManager::Shutdown()
{
  for (auto &[Path, Asset] : m_Assets)
    Asset->Shutdown();

  m_Assets.clear();
}

std::shared_ptr<CModel> CResourceManager::LoadModel(const std::filesystem::path &_Path)
{
  if (!IsFormatSupported(_Path))
    return nullptr;

  auto Iter = m_Assets.find(_Path);
  if (Iter == m_Assets.end())
  {
    std::shared_ptr<IAsset> Model = std::make_shared<CModel>(std::make_unique<CTinyGLTFParseStrategy>());
    if (Model->Load(_Path, CPasskey(this)))
      Iter = m_Assets.emplace(_Path.string(), std::move(Model)).first;
    else
      return nullptr;
  }

  return std::static_pointer_cast<CModel>(Iter->second);
}

std::shared_ptr<CShader> CResourceManager::LoadShader(const std::string &_Name)
{
  const std::filesystem::path ShaderPath = CConfig::Instance().GetShadersDir() / _Name;

  auto Iter = m_Assets.find(ShaderPath);
  if (Iter == m_Assets.end())
  {
    std::shared_ptr<IAsset> Shader = std::make_shared<CShader>();
    if (Shader->Load(ShaderPath, CPasskey(this)))
      Iter = m_Assets.emplace(ShaderPath.string(), std::move(Shader)).first;
    else
      return nullptr;
  }

  return std::static_pointer_cast<CShader>(Iter->second);
}

std::shared_ptr<CTextureBase> CResourceManager::GetFallbackTexture()
{
  return LoadTexture(GetDefaultTexturePath());
}

std::shared_ptr<CTextureBase> CResourceManager::LoadTexture(const std::filesystem::path &_Path)
{
  auto Iter = m_Assets.find(_Path);
  if (Iter == m_Assets.end())
  {
    std::shared_ptr<IAsset> Texture = std::make_shared<CTexture>();
    if (Texture->Load(_Path, CPasskey(this)))
      Iter = m_Assets.emplace(_Path.string(), std::move(Texture)).first;
    else
      Iter = m_Assets.find(GetDefaultTexturePath());
  }

  return std::static_pointer_cast<CTextureBase>(Iter->second);
}

std::shared_ptr<CTextureBase> CResourceManager::LoadCubemap(const std::filesystem::path &_Path)
{
  auto Iter = m_Assets.find(_Path);
  if (Iter == m_Assets.end())
  {
    std::shared_ptr<IAsset> Texture = std::make_shared<CCubemap>();
    if (Texture->Load(_Path, CPasskey(this)))
      Iter = m_Assets.emplace(_Path.string(), std::move(Texture)).first;
    else
      Iter = m_Assets.find(GetDefaultTexturePath());
  }

  return std::static_pointer_cast<CTextureBase>(Iter->second);
}

std::shared_ptr<CTextureBase> CResourceManager::CreateTexture(const std::string &_Name,
                                                              const TTextureParams &_Params)
{
  if (_Name.empty())
  {
    CLogger::Log(ELogType::Error, "[CResourceManager] Texture name is empty");
    return GetFallbackTexture();
  }

  auto Iter = m_Assets.find(_Name);
  if (Iter == m_Assets.end())
  {
    std::shared_ptr<CTexture> Texture = std::make_shared<CTexture>();
    if (Texture->Generate(_Params, CPasskey(this)))
      Iter = m_Assets.emplace(_Name, std::move(Texture)).first;
    else
      return GetFallbackTexture();
  }

  return std::static_pointer_cast<CTextureBase>(Iter->second);
}

std::filesystem::path CResourceManager::GetDefaultTexturePath()
{
  return CConfig::Instance().GetAssetsDir() / "textures/default.jpg";
}

std::filesystem::path CResourceManager::GetFallbackTexturePath()
{
  return CConfig::Instance().GetAssetsDir() / "textures/fallback.jpg";
}