#if DEV_STAGE

#include "ComponentDataWindow.h"

#include <common/UnorderedVector.h>
#include "../renderer/ComponentRenderer.h"
#include "interfaces/WorldEditor.h"
#include <ecs/ComponentView.h>
#include <imgui/imgui.h>
#include <algorithm>

namespace editor
{

CComponentDataWindow::CComponentDataWindow(IWorldEditor &_WorldEditor, CComponentRenderer &_ComponentRenderer) :
    m_WorldEditor(_WorldEditor),
    m_ComponentRenderer(_ComponentRenderer)
{
}

void CComponentDataWindow::Render(const std::optional<ecs::TEntity> &_SelectedEntity)
{
  if (ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
  {
    if (!_SelectedEntity.has_value())
    {
      ImGui::TextUnformatted("Select an entity to inspect.");
    }
    else
    {
      const CUnorderedVector<ecs::TComponentView> Components = m_WorldEditor.GetEntityComponents(_SelectedEntity.value());
      for (const ecs::TComponentView &ComponentView : Components)
      {
        if (ImGui::CollapsingHeader(ComponentView.Name.c_str()))
          m_ComponentRenderer.Render(ComponentView);
      }
    }

    const ImVec2 Available = ImGui::GetContentRegionAvail();
    m_Size                 = TVector2i(static_cast<int>(Available.x), static_cast<int>(Available.y));
  }

  ImGui::End();
}

} // namespace editor

#endif
