#pragma once

#if DEV_STAGE

#include "EditorWIndow.h"
#include <common/MathTypes.h>

namespace editor
{

class CGlobalParamsWindow : public IEditorWindow
{
public:
  void Render();

  std::string GetName() const override
  {
    return "Global Parameters";
  }

  TVector2i GetSize() const override
  {
    return m_Size;
  }

private:
  TVector2i m_Size;
};

} // namespace editor

#endif
