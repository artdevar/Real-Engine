#pragma once

#include <filesystem>
#include <memory>

class CModel;
class CShader;
class CTextureBase;

namespace resource
{

    std::shared_ptr<CModel> LoadModel(std::filesystem::path _Path);
    std::shared_ptr<CShader> LoadShader(std::filesystem::path _Path);
    std::shared_ptr<CTextureBase> LoadTexture(std::filesystem::path _Path);
    std::shared_ptr<CTextureBase> LoadCubemap(std::filesystem::path _Path);

}