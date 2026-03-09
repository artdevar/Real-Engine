#pragma once

#if DEV_STAGE

#include <ecs/Core.h>
#include "EditorWindow.h"
#include "GizmoRenderer.h"

class IWorldEditor;

namespace editor
{

class CViewportWindow : public IEditorWindow
{
public:
  CViewportWindow(IWorldEditor &_WorldEditor);

  std::string GetName() const override;
  TVector2i GetSize() const override;

  void Render(const std::optional<ecs::TEntity> &_SelectedEntity);

private:
  IWorldEditor  &m_WorldEditor;
  CGizmoRenderer m_GizmoRenderer;
  TVector2i      m_Size;
};

} // namespace editor

#endif
