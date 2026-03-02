#pragma once

#if DEV_STAGE

#include "EditorWIndow.h"
#include <ecs/Core.h>
#include <ecs/ComponentView.h>
#include <imgui/imgui.h>
#include <optional>

class IWorldEditor;

namespace editor
{

class CComponentRenderer;

class CComponentDataWindow : public IEditorWindow
{
public:
  CComponentDataWindow(IWorldEditor &_WorldEditor, CComponentRenderer &_ComponentRenderer);

  void Render(const std::optional<ecs::TEntity> &_SelectedEntity);

  std::string GetName() const override
  {
    return "Properties";
  }

  TVector2i GetSize() const override
  {
    return m_Size;
  }

private:
  IWorldEditor       &m_WorldEditor;
  CComponentRenderer &m_ComponentRenderer;
  TVector2i           m_Size;
};

} // namespace editor

#endif
