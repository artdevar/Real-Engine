#include "Resource.h"
#include "engine/Engine.h"
#include "engine/ResourceManager.h"

static CResourceManager *Get()
{
    return CEngine::Instance().GetResourceManager().get();
}

namespace resource
{

    std::shared_ptr<CModel> LoadModel(std::filesystem::path _Path)
    {
        return Get()->LoadModel(std::move(_Path));
    }

    std::shared_ptr<CShader> LoadShader(std::filesystem::path _Path)
    {
        return Get()->LoadShader(std::move(_Path));
    }

    std::shared_ptr<CTextureBase> LoadTexture(std::filesystem::path _Path)
    {
        return Get()->LoadTexture(std::move(_Path));
    }

    std::shared_ptr<CTextureBase> LoadCubemap(std::filesystem::path _Path)
    {
        return Get()->LoadCubemap(std::move(_Path));
    }

}
