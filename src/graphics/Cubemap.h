#pragma once

#include "TextureBase.h"

class CCubemap :
  public CTextureBase
{
public:

  CCubemap();

  void Init(const shared::TTextureInitParams & _Params) override;

};