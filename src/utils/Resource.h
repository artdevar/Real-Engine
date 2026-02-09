#pragma once

class CModel;
class CShader;
class CTextureBase;
struct TTextureParams;

namespace resource
{

    std::shared_ptr<CShader> LoadShader(const std::string &_Name);
    std::shared_ptr<CModel> LoadModel(const std::filesystem::path &_Path);
    std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path &_Path);
    std::shared_ptr<CTextureBase> LoadCubemap(const std::filesystem::path &_Path);
    std::shared_ptr<CTextureBase> CreateTexture(const std::string &_Name,
                                                const TTextureParams &_Params);
    std::shared_ptr<CTextureBase> GetFallbackTexture();

}