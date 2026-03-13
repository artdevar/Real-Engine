#pragma once

#include <common/MathTypes.h>
#include "render/Buffer.h"
#include "assets/Texture.h"

struct TRenderTarget
{
  using TColorHandle = std::shared_ptr<CTextureBase>;
  using TDepthHandle = std::shared_ptr<CTextureBase>;

  CFrameBuffer FrameBuffer;

  TColorHandle Color;
  TDepthHandle Depth;

  TVector2i Size;
};