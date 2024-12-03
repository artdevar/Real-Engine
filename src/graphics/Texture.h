#pragma once

#include "TextureBase.h"

class CTexture :
  public CTextureBase
{
public:

  CTexture();

  void Init(const shared::TTextureInitParams & _Params) override;

};