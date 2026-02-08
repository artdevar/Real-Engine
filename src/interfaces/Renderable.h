#pragma once

class CRenderer;

class IRenderable
{
public:
  virtual ~IRenderable() = default;

  void Render(CRenderer &_Renderer)
  {
    if (ShouldBeRendered())
      RenderInternal(_Renderer);
  }

protected:
  virtual bool ShouldBeRendered() const = 0;

  virtual void RenderInternal(CRenderer &_Renderer) = 0;
};