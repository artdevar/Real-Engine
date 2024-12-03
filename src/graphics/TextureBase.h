#pragma once

#include <glad/glad.h>
#include "interfaces/Shutdownable.h"
#include "Shared.h"

class CTextureBase :
  public IShutdownable
{
public:

  CTextureBase(GLenum _Target);

  void Shutdown() override;

  virtual void Init(const shared::TTextureInitParams & _Params) = 0;

  void Bind(GLenum _TextureUnit);

  void Unbind();

  GLuint Get() const;

  bool IsValid() const;

protected:

        GLuint m_ID;
  const GLenum m_Target;

};