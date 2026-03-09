#if DEV_STAGE

#include "ViewportWindow.h"
#include "GizmoRenderer.h"
#include "engine/Engine.h"
#include "engine/Config.h"
#include "interfaces/RenderPipeline.h"
#include "interfaces/WorldEditor.h"
#include "utils/Event.h"
#include <common/MathTypes.h>
#include <imgui/imgui.h>

namespace editor
{

CViewportWindow::CViewportWindow(IWorldEditor &_WorldEditor) :
    m_WorldEditor(_WorldEditor),
    m_GizmoRenderer(_WorldEditor)
{
}

std::string CViewportWindow::GetName() const
{
  return "Viewport";
}

TVector2i CViewportWindow::GetSize() const
{
  return m_Size;
}

void CViewportWindow::Render(const std::optional<ecs::TEntity> &_SelectedEntity)
{
  if (ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
  {
    const bool      IsGizmoEnabled = CConfig::Instance().GetGizmoEnabled();
    const uint32_t  TextureID      = CEngine::Instance().GetRenderPipeline()->GetRenderTextureID();
    const ImVec2    Available      = ImGui::GetContentRegionAvail();
    const ImVec2    Pos            = ImGui::GetCursorScreenPos();
    const TVector2i NewSize        = TVector2i(static_cast<int>(Available.x), static_cast<int>(Available.y));

    if (NewSize != m_Size)
    {
      m_Size = NewSize;
      event::Notify(TEventType::ViewportResized, NewSize);
    }

    if (TextureID != 0 && Available.x > 0.0f && Available.y > 0.0f)
      ImGui::Image(static_cast<ImTextureID>(static_cast<uintptr_t>(TextureID)), Available, ImVec2(0, 1), ImVec2(1, 0));
    else
      ImGui::TextUnformatted("Render texture unavailable");

    if (IsGizmoEnabled && _SelectedEntity.has_value())
    {
      ImGui::SetCursorScreenPos(Pos);
      m_GizmoRenderer.Render(_SelectedEntity.value(), TRectf(Pos.x, Pos.y, Available.x, Available.y));
    }
  }

  ImGui::End();
}

} // namespace editor

#endif