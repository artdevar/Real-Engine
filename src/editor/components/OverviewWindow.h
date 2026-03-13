#pragma once

#if DEV_STAGE

#include "EditorWindow.h"
#include <common/containers/StaticArray.h>
#include <imgui/imgui.h>

namespace editor
{

class COverviewWindow : public IEditorWindow
{
public:
  std::string GetName() const override;
  TVector2i GetSize() const override;

  void Render();

private:
  TVector2i m_Size;
};

} // namespace editor

#endif
