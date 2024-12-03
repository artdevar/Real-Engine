#include "Cubemap.h"

CCubemap::CCubemap() : CTextureBase(GL_TEXTURE_CUBE_MAP)
{
}

void CCubemap::Init(const shared::TTextureInitParams &_Params)
{
  glGenTextures(1, &m_ID);
  glBindTexture(m_Target, m_ID);

  for (int i = 0; i < _Params.Images.size(); ++i)
  {
    const shared::TImage & Image = _Params.Images[i];
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, Image.Width, Image.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Image.Data);
  }

  glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
