#pragma once

#include "engine/Passkey.h"
#include "assets/TextureParams.h"

class CResourceManager;

class IGeneratableTexture
{
public:
  virtual ~IGeneratableTexture() = default;

  virtual bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) = 0;
};
