#pragma once

class IRenderer;

class IRenderable
{
public:
  virtual ~IRenderable() = default;

  void Render(IRenderer &_Renderer)
  {
    if (ShouldBeRendered())
      RenderInternal(_Renderer);
  }

protected:
  virtual void RenderInternal(IRenderer &_Renderer) = 0;
  virtual bool ShouldBeRendered() const = 0;
};