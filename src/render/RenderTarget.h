#pragma once

#include <common/MathTypes.h>
#include "render/Buffer.h"
#include "assets/Texture.h"

struct TRenderTarget
{
  using TTexture     = std::shared_ptr<CTextureBase>;
  using TDepthHandle = std::variant<TTexture, CRenderBuffer>;

  CFrameBuffer FrameBuffer;
  TVector2i    Size;

  TDepthHandle Depth;
  TTexture     Color;
  TTexture     Velocity;
};