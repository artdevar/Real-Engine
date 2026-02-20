#pragma once

#include "engine/MathCore.h"
#include "render/RenderTypes.h"
#include "render/ShaderTypes.h"
#include <memory>
#include <string_view>

class CShader;
class CCamera;
class CTextureBase;

class IRenderer
{
public:
  virtual ~IRenderer() = default;

  virtual void CheckErrors() = 0;

  virtual void Clear(EClearFlags _ClearFlags)   = 0;
  virtual void ClearColor(const TColor &_Color) = 0;

  virtual void SetCamera(const std::shared_ptr<CCamera> &_Camera) = 0;
  virtual const std::shared_ptr<CCamera> &GetCamera() const       = 0;

  virtual void SetShader(const std::shared_ptr<CShader> &_Shader)            = 0;
  virtual void SetUniform(std::string_view _Name, const UniformType &_Value) = 0;
  virtual const std::shared_ptr<CShader> &GetShader() const                  = 0;

  virtual void SetViewport(TVector2i _Viewport) = 0;
  virtual TVector2i GetViewport() const         = 0;

  virtual void DrawElements(EPrimitiveMode _Mode, int _Count, EIndexType _IndexType, const void *_Offset = nullptr) = 0;
  virtual void DrawArrays(EPrimitiveMode _Mode, int _Count)                                                         = 0;

  virtual void SetBlending(EAlphaMode _Mode) = 0;
  virtual void SetCullFace(ECullMode _Mode)  = 0;
  virtual void SetDepthTest(bool _Enable)    = 0;
  virtual void SetDepthFunc(int _Func)       = 0;
  virtual void SetDepthMask(bool _Flag)      = 0;
};