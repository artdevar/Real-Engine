#include "Texture.h"

CTexture::CTexture() : CTextureBase(GL_TEXTURE_2D)
{
}

void CTexture::Init(const shared::TTextureInitParams &_Params)
{
  constexpr GLenum Formats[] = { GL_RED, GL_RED, GL_RGB, GL_RGBA };
  const     GLenum Format    = Formats[_Params.Image.Channels - 1];

  glGenTextures(1, &m_ID);
  glBindTexture(m_Target, m_ID);
  glTexImage2D(m_Target, 0, Format, _Params.Image.Width, _Params.Image.Height, 0, Format, GL_UNSIGNED_BYTE, _Params.Image.Data);
  glGenerateMipmap(m_Target);

  glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
