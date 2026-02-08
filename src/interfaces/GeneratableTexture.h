#pragma once

#include "graphics/TextureParams.h"
#include "engine/Passkey.h"

class CResourceManager;

class IGeneratableTexture
{
public:
    virtual ~IGeneratableTexture() = default;

    virtual bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) = 0;
};
