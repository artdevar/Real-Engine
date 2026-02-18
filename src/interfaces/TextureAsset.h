#include "Asset.h"
#include "assets/TextureParams.h"

class ITextureAsset : public IAsset
{
public:
  using IAsset::Load;
  virtual bool Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>) = 0;
};