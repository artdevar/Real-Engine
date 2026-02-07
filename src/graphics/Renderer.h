#pragma once

#include <glad/glad.h>
#include <memory>
#include <string_view>
#include "ShaderTypes.h"
#include "Buffer.h"

class CShader;
class CCamera;

enum ClearFlags
{
  Clear_Color = 0x01,
  Clear_Depth = 0x02,
  Clear_Stencil = 0x04
};

class CRenderer final
{
public:
  CRenderer();

  void BeginFrame(float _R, float _G, float _B, float _A, unsigned int _ClearFlags = Clear_Color | Clear_Depth);
  void EndFrame();
  void CheckErrors();

  void Clear(GLbitfield _Mask);
  void ClearColor(GLfloat _R, GLfloat _G, GLfloat _B, GLfloat _A);

  void SetViewport(GLsizei _Width, GLsizei _Height);

  void DrawElements(GLenum _Mode, GLsizei _Count, GLenum _Type);
  void DrawArrays(GLenum _Mode, GLsizei _Count);

  void SetCamera(const std::shared_ptr<CCamera> &_Camera);
  const std::shared_ptr<CCamera> &GetCamera() const;

  void SetShader(const std::shared_ptr<CShader> &_Shader);
  const std::shared_ptr<CShader> &GetShader() const;

  void SetUniform(std::string_view _Name, const UniformType &_Value);
  void SetLightingData(TShaderLighting &&_Data);

private:
  void InitShaderValues();

private:
  std::shared_ptr<CCamera> m_Camera;
  std::shared_ptr<CShader> m_CurrentShader;

  TShaderLighting m_Lighting;
  CUniformBuffer m_LightingUBO;
};