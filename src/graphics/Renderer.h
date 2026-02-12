#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <memory>
#include <string_view>
#include "ShaderTypes.h"
#include "Buffer.h"

class CShader;
class CCamera;

enum ClearFlags : GLbitfield
{
  Clear_Color = GL_COLOR_BUFFER_BIT,
  Clear_Depth = GL_DEPTH_BUFFER_BIT,
  Clear_Stencil = GL_STENCIL_BUFFER_BIT
};

// OpenGL Renderer
class CRenderer final
{
public:
  CRenderer();

  void BeginFrame(float _R, float _G, float _B, float _A, ClearFlags _ClearFlags = static_cast<ClearFlags>(Clear_Color | Clear_Depth));
  void EndFrame();
  void CheckErrors();

  void Clear(GLbitfield _Mask);
  void ClearColor(GLfloat _R, GLfloat _G, GLfloat _B, GLfloat _A);

  void SetViewport(GLsizei _Width, GLsizei _Height);
  glm::ivec2 GetViewport() const;

  void DrawElements(GLenum _Mode, GLsizei _Count, GLenum _Type, const void *_Offset = nullptr);
  void DrawArrays(GLenum _Mode, GLsizei _Count);

  void SetCamera(const std::shared_ptr<CCamera> &_Camera);
  const std::shared_ptr<CCamera> &GetCamera() const;

  void SetShader(const std::shared_ptr<CShader> &_Shader);
  void SetUniform(std::string_view _Name, const UniformType &_Value);
  const std::shared_ptr<CShader> &GetShader() const;

  void SetLightingData(TShaderLighting &&_Data);
  glm::mat4 GetLightSpaceMatrix() const;

  void SetBlending(bool _Enabled);
  void SetCullFace(GLenum _Mode);

private:
  void InitShaderValues();

  static std::string GetGLErrorDescription(GLenum _Error);

public:
  std::shared_ptr<CCamera> m_Camera;
  std::shared_ptr<CShader> m_CurrentShader;

  TShaderLighting m_Lighting;
  CUniformBuffer m_LightingUBO;
  GLenum m_CullingMode = -1;

  mutable glm::mat4 m_LightSpaceMatrix;
  mutable bool m_LightSpaceMatrixDirty = true;
};