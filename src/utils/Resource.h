#pragma once

#include "assets/TextureParams.h"

class CModel;
class CShader;
class CTextureBase;

namespace resource
{

std::shared_ptr<CShader> LoadShader(const std::string &_Name);
std::shared_ptr<CModel> LoadModel(const std::filesystem::path &_Path);
std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path &_Path);
std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path &_Path, const TTextureParams &_Params);
std::shared_ptr<CTextureBase> LoadCubemap(const std::filesystem::path &_Path);
std::shared_ptr<CTextureBase> CreateTexture(const std::string &_Name, const TTextureParams &_Params);
std::shared_ptr<CTextureBase> RecreateTexture(const std::string &_Name, const TTextureParams &_Params);
std::shared_ptr<CTextureBase> GetDefaultTexture(ETextureType _TextureType);
void MarkUnused(const std::string &_Name);

} // namespace resource