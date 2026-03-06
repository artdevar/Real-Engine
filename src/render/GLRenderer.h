#pragma once

#include <glad/glad.h>
#include "Buffer.h"
#include "RenderTypes.h"
#include "ShaderTypes.h"
#include "interfaces/Renderer.h"

class COpenGLRenderer final : public IRenderer
{
public:
  void OnFrameBegin() override;
  void CheckErrors() override;

  void Clear(EClearFlags _ClearFlags) override;
  void ClearColor(const TColor &_Color) override;

  void SetViewport(TVector2i _Viewport) override;
  TVector2i GetViewport() const override;

  void DrawElements(EPrimitiveMode _Mode, int _Count, EIndexType _IndexType, const void *_Offset = nullptr) override;
  void DrawArrays(EPrimitiveMode _Mode, int _Count) override;

  void SetCamera(const std::shared_ptr<CCamera> &_Camera) override;
  const std::shared_ptr<CCamera> &GetCamera() const override;

  void SetShader(const std::shared_ptr<CShader> &_Shader) override;
  void SetUniform(std::string_view _Name, const UniformType &_Value) override;
  const std::shared_ptr<CShader> &GetShader() const override;

  void SetBlending(EAlphaMode _Mode) override;
  void SetCullFace(ECullMode _Mode) override;
  void SetDepthTest(bool _Enable) override;
  void SetDepthFunc(int _Func) override;
  void SetDepthMask(bool _Flag) override;

  uint32_t GetDrawCallsCount() const override;
  uint32_t GetVerticesCount() const override;
  uint32_t GetIndicesCount() const override;
  uint32_t GetTrianglesCount() const override;
  uint32_t GetLinesCount() const override;
  uint32_t GetPointsCount() const override;

private:
  static std::string GetGLErrorDescription(GLenum _Error);

public:
  std::shared_ptr<CCamera> m_Camera;
  std::shared_ptr<CShader> m_CurrentShader;

  EAlphaMode m_AlphaMode   = static_cast<EAlphaMode>(-1);
  ECullMode  m_CullingMode = static_cast<ECullMode>(-1);

  uint32_t m_DrawCallsCount = 0;
  uint32_t m_VerticesCount  = 0;
  uint32_t m_IndicesCount   = 0;
  uint32_t m_TrianglesCount = 0;
  uint32_t m_LinesCount     = 0;
  uint32_t m_PointsCount    = 0;
};