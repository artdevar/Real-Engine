#include "pch.h"
#include "Resource.h"
#include "graphics/TextureParams.h"
#include "engine/Engine.h"
#include "engine/ResourceManager.h"

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

    std::shared_ptr<CTextureBase> LoadTexture(const std::filesystem::path &_Path)
    {
        return Get()->LoadTexture(_Path);
    }

    std::shared_ptr<CTextureBase> LoadCubemap(const std::filesystem::path &_Path)
    {
        return Get()->LoadCubemap(_Path);
    }

    std::shared_ptr<CTextureBase> CreateTexture(const std::string &_Name, const TTextureParams &_Params)
    {
        return Get()->CreateTexture(_Name, _Params);
    }

    std::shared_ptr<CTextureBase> GetDefaultBasicTexture()
    {
        return Get()->GetDefaultBasicTexture();
    }

    std::shared_ptr<CTextureBase> GetDefaultNormalTexture()
    {
        return Get()->GetDefaultNormalTexture();
    }

    std::shared_ptr<CTextureBase> GetDefaultRoughnessTexture()
    {
        return Get()->GetDefaultRoughnessTexture();
    }

}
