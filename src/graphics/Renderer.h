#pragma once

#include <glad/glad.h>
#include <memory>
#include "ShaderTypes.h"
#include "Buffer.h"

class CShader;
class CCamera;

class CRenderer final
{
public:

  CRenderer();

  void Clear(GLbitfield _Mask);
  void ClearColor(GLfloat _R, GLfloat _G, GLfloat _B, GLfloat _A);

  void SetViewport(GLsizei _Width, GLsizei _Height);

  void DrawElements(GLenum _Mode, GLsizei _Count, GLenum _Type);
  void DrawArrays(GLenum _Mode, GLsizei _Count);

  void SetCamera(const std::shared_ptr<CCamera> & _Camera);
  const std::shared_ptr<CCamera> & GetCamera() const;

  void SetShader(const std::shared_ptr<CShader> & _Shader);
  const std::shared_ptr<CShader> & GetShader() const;

  void SetLightingData(TShaderLighting && _Data);

private:

  void InitShaderValues();

private:

  std::shared_ptr<CCamera> m_Camera;
  std::shared_ptr<CShader> m_CurrentShader;

  TShaderLighting m_Lighting;
  CUniformBuffer  m_LightingUBO;

};