#pragma once

#include <ecs/Core.h>
#include <common/MathTypes.h>
#include <common/containers/StaticArray.h>

class IWorldEditor;

namespace editor
{

class CGizmoRenderer
{
public:
  CGizmoRenderer(IWorldEditor &_WorldEditor);

  void Render(ecs::TEntity _Entity, TRectf _ViewportRect);

private:
  IWorldEditor        &m_WorldEditor;
  CStaticArray<int, 3> m_Operations;
  int                  m_CurrentOperation;
};

} // namespace editor