#pragma once

#if DEV_STAGE

#include "EditorWIndow.h"
#include <imgui/imgui.h>

namespace editor
{

class CViewportWindow : public IEditorWindow
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
