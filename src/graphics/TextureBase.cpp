#include "TextureBase.h"

CTextureBase::CTextureBase(GLenum _Target) :
  m_ID(0u),
  m_Target(_Target)
{}

void CTextureBase::Shutdown()
{
  if (IsValid())
  {
    glDeleteTextures(1, &m_ID);
    m_ID = 0u;
  }
}

void CTextureBase::Bind(GLenum _TextureUnit)
{
  glActiveTexture(_TextureUnit);
  glBindTexture(m_Target, Get());
}

void CTextureBase::Unbind()
{
  //glActiveTexture(0); opengl error
  glBindTexture(m_Target, 0);
}

GLuint CTextureBase::Get() const
{
  return m_ID;
}

bool CTextureBase::IsValid() const
{
  return m_ID != 0u;
}
