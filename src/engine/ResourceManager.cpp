#include "ResourceManager.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"
#include "graphics/Cubemap.h"
#include "graphics/ModelLoader.h"
#include "utils/Json.h"
#include "utils/Logger.h"
#include "stb_image.h"
#include <nlohmann/json.hpp>
#include <fstream>

void CResourceManager::Shutdown()
{
  for (auto & [Path, Model] : m_Models)
    Model->Meshes.clear();

  for (auto & [Path, Shader] : m_Shaders)
    Shader->Shutdown();
  m_Shaders.clear();

  for (auto & [Path, Texture] : m_Textures)
    Texture->Shutdown();
  m_Textures.clear();
}

std::shared_ptr<TModel> CResourceManager::LoadModel(const std::filesystem::path & _Path)
{
  const std::string Path = _Path.string();

  auto Iter = m_Models.find(Path);
  if (Iter == m_Models.end())
  {
    TLoadParams Params;
    if (_Path.extension() == ".json")
    {
      nlohmann::json JsonContent = utils::ParseJson(Path);
      Params.ModelPath = _Path.parent_path() / JsonContent["ModelPath"].get<std::string>();
      Params.FlipUVs   = JsonContent["FlipUVs"].get<bool>();
    }
    else
    {
      Params.ModelPath = Path;
      Params.FlipUVs   = true;
    }

    Iter = m_Models.emplace(Path, CModelLoader::LoadModel(Params)).first;
  }

  return Iter->second;
}

std::shared_ptr<CShader> CResourceManager::LoadShader(const std::string & _Path)
{
  auto Iter = m_Shaders.find(_Path);
  if (Iter == m_Shaders.end())
  {
    std::shared_ptr<CShader> Shader = std::make_shared<CShader>();
    if (Shader->Init(_Path))
      Iter = m_Shaders.emplace(_Path, std::move(Shader)).first;
  }

  return Iter->second;
}

std::shared_ptr<CTextureBase> CResourceManager::LoadTexture(const std::filesystem::path & _Path)
{
  const std::string Path = _Path.string();

  auto Iter = m_Textures.find(Path);
  if (Iter == m_Textures.end())
  {
    shared::TTextureInitParams Params;
    Params.Image.Data = stbi_load(Path.c_str(), &Params.Image.Width, &Params.Image.Height, &Params.Image.Channels, 0);
    if (Params.Image.Data)
    {
      std::shared_ptr<CTextureBase> Texture = std::make_shared<CTexture>();
      Texture->Init(Params);
      assert(Texture->IsValid());
      stbi_image_free(Params.Image.Data);
      Iter = m_Textures.emplace(Path, std::move(Texture)).first;

      CLogger::Log(ELogType::Info, std::format("Texture '{}' loaded\n", Path));
    }
    else
    {
      CLogger::Log(ELogType::Error, std::format("Loading texture '{}' is failed\n", Path));
    }
  }

  return Iter->second;
}

std::shared_ptr<CTextureBase> CResourceManager::LoadCubemap(const std::filesystem::path & _Path)
{
  const std::string Path = _Path.string();

  auto Iter = m_Textures.find(Path);
  if (Iter == m_Textures.end())
  {
    assert(_Path.extension() == ".json");
    const nlohmann::json JsonContent = utils::ParseJson(Path);
    const auto           Faces       = JsonContent["Faces"].get<std::vector<std::string>>();
    assert(Faces.size() == shared::CUBEMAP_FACES);

    bool IsLoadingFailed = false;

    stbi_set_flip_vertically_on_load(false);

    shared::TTextureInitParams Params;
    for (int i = 0; i < Faces.size(); ++i)
    {
      Params.Images[i].Data = stbi_load((_Path.parent_path() / Faces[i]).c_str(), &Params.Images[i].Width, &Params.Images[i].Height, &Params.Images[i].Channels, 0);
      if (IsLoadingFailed = (Params.Images[i].Data == nullptr))
        break;
    }

    stbi_set_flip_vertically_on_load(true);

    if (!IsLoadingFailed)
    {
      std::shared_ptr<CTextureBase> Texture = std::make_shared<CCubemap>();
      Texture->Init(Params);
      assert(Texture->IsValid());
      Iter = m_Textures.emplace(Path, std::move(Texture)).first;
    }
    else
    {
      CLogger::Log(ELogType::Error, std::format("Loading cubemap '{}' is failed\n", Path));
    }

    for (int i = 0; i < Params.Images.size(); ++i)
      stbi_image_free(Params.Images[i].Data);
  }

  return Iter->second;
}