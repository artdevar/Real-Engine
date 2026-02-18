#include "pch.h"

#include "Passkey.h"
#include "ResourceManager.h"
#include "engine/Config.h"
#include "assets/Model.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "assets/TextureParams.h"
#include "assets/TinyGLTFParseStrategy.h"
#include "utils/Logger.h"

static inline bool operator<(const std::string &_L, const std::filesystem::path &_R)
{
  return _L < _R.c_str();
}

static bool IsFormatSupported(const std::filesystem::path &_Path)
{
  const std::string Format = _Path.extension().string();
  if (Format == ".gltf")
    return true;

  CLogger::Log(ELogType::Error, "Unsupported model format '{}'", Format);
  return false;
}

void CResourceManager::Init()
{
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

std::shared_ptr<CTextureBase> CResourceManager::GetDefaultTexture(ETextureType _TextureType)
{
  return LoadTexture(GetDefaultTexturePath(_TextureType));
}

std::shared_ptr<CTextureBase> CResourceManager::LoadTexture(const std::filesystem::path &_Path)
{
  auto Iter = m_Assets.find(_Path);
  if (Iter == m_Assets.end())
  {
    std::shared_ptr<CTexture> Texture = std::make_shared<CTexture>();
    if (Texture->Load(_Path, CPasskey(this)))
      Iter = m_Assets.emplace(_Path.string(), std::move(Texture)).first;
    else
      return nullptr;
  }

  return std::static_pointer_cast<CTextureBase>(Iter->second);
}

std::shared_ptr<CTextureBase> CResourceManager::LoadTexture(const std::filesystem::path &_Path, const TTextureParams &_Params)
{
  auto Iter = m_Assets.find(_Path);
  if (Iter == m_Assets.end())
  {
    std::shared_ptr<CTexture> Texture = std::make_shared<CTexture>();
    if (Texture->Load(_Path, _Params, CPasskey(this)))
      Iter = m_Assets.emplace(_Path.string(), std::move(Texture)).first;
    else
      return nullptr;
  }

  return std::static_pointer_cast<CTextureBase>(Iter->second);
}

std::shared_ptr<CTextureBase> CResourceManager::LoadCubemap(const std::filesystem::path &_Path)
{
  auto Iter = m_Assets.find(_Path);
  if (Iter == m_Assets.end())
  {
    std::shared_ptr<CCubemap> Texture = std::make_shared<CCubemap>();
    if (Texture->Load(_Path, CPasskey(this)))
      Iter = m_Assets.emplace(_Path.string(), std::move(Texture)).first;
    else
      return nullptr;
  }

  return std::static_pointer_cast<CTextureBase>(Iter->second);
}

std::shared_ptr<CTextureBase> CResourceManager::CreateTexture(const std::string &_Name, const TTextureParams &_Params)
{
  if (_Name.empty())
  {
    CLogger::Log(ELogType::Error, "[CResourceManager] Texture name is empty");
    return nullptr;
  }

  auto Iter = m_Assets.find(_Name);
  if (Iter == m_Assets.end())
  {
    std::shared_ptr<CTexture> Texture = std::make_shared<CTexture>();
    if (Texture->Generate(_Params, CPasskey(this)))
      Iter = m_Assets.emplace(_Name, std::move(Texture)).first;
    else
      return nullptr;
  }

  return std::static_pointer_cast<CTextureBase>(Iter->second);
}

std::filesystem::path CResourceManager::GetDefaultTexturePath(ETextureType _TextureType)
{
  switch (_TextureType)
  {
  case ETextureType::BasicColor:
    return CConfig::Instance().GetTexturesDir() / "basic.jpg";
  case ETextureType::Normal:
    return CConfig::Instance().GetTexturesDir() / "normal.png";
  case ETextureType::Roughness:
    return CConfig::Instance().GetTexturesDir() / "roughness.jpg";
  case ETextureType::Emissive:
    return CConfig::Instance().GetTexturesDir() / "emissive.jpg";
  default:
    return CConfig::Instance().GetTexturesDir() / "missing.jpg";
  }
}
