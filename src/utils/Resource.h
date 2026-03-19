#pragma once

#include "assets/TextureParams.h"

class CModel;
class CShader;
class CTexture;

namespace resource
{

std::shared_ptr<CShader> LoadShader(const std::string &_Name);
std::shared_ptr<CModel> LoadModel(const std::filesystem::path &_Path);
std::shared_ptr<CTexture> LoadTexture(const std::filesystem::path &_Path);
std::shared_ptr<CTexture> LoadTexture(const std::filesystem::path &_Path, const TTextureParams &_Params);
std::shared_ptr<CTexture> LoadCubemap(const std::filesystem::path &_Path);
std::shared_ptr<CTexture> CreateCubemap(const std::string &_Name, const TTextureParams &_Params);
std::shared_ptr<CTexture> CreateTexture(const std::string &_Name, const TTextureParams &_Params);
std::shared_ptr<CTexture> RecreateTexture(const std::string &_Name, const TTextureParams &_Params);
std::shared_ptr<CTexture> GetDefaultTexture(ETextureType _TextureType);
void MarkUnused(const std::string &_Name);
void UnloadUnusedAssets();

} // namespace resource