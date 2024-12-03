#pragma once

class CRenderer;

class IRenderable
{
public:

  virtual ~IRenderable() = default;

  virtual void Render(CRenderer & _Renderer) = 0;
};