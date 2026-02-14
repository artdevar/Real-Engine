#include "ComponentsFactory.h"

namespace ecs
{
  void CComponentsFactory::CreateSkyboxComponent(const std::shared_ptr<CTextureBase> &_Skybox, TSkyboxComponent &_Component)
  {
    _Component.SkyboxTexture = _Skybox;
  }
}