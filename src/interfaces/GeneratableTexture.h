#pragma once

#include "assets/TextureParams.h"
#include <common/Passkey.h>

class CResourceManager;

class IGeneratableTexture
{
public:
  virtual ~IGeneratableTexture() = default;

  virtual bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) = 0;
};
