#include "../ComponentsFactory.h"

#include "assets/TextureParams.h"
#include "utils/Resource.h"

namespace ecs
{

void CComponentsFactory::CreateSkyboxComponent(const std::shared_ptr<CTextureBase> &_EquirectMap,
                                               const std::shared_ptr<CTextureBase> &_Skybox,
                                               TSkyboxComponent                    &_Component)
{
  _Component.EquirectangularTexture = _EquirectMap;
  _Component.SkyboxTexture          = _Skybox;
}

} // namespace ecs