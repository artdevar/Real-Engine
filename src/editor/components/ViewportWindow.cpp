#if DEV_STAGE

#include "ViewportWindow.h"
#include "engine/Engine.h"
#include "utils/Event.h"
#include <imgui/imgui.h>

namespace editor
{

static TVector2i size{};

std::string CViewportWindow::GetName() const
{
  return "Viewport";
}

TVector2i CViewportWindow::GetSize() const
{
  return m_Size;
}

void CViewportWindow::Render()
{
  if (ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
  {
    const uint32_t  TextureID = CEngine::Instance().GetRenderTextureID();
    const ImVec2    Available = ImGui::GetContentRegionAvail();
    const TVector2i NewSize   = TVector2i(static_cast<int>(Available.x), static_cast<int>(Available.y));

    if (NewSize != m_Size)
    {
      m_Size = NewSize;
      event::Notify(TEventType::ViewportResized, NewSize);
    }

    if (TextureID != 0 && Available.x > 0.0f && Available.y > 0.0f)
      ImGui::Image(static_cast<ImTextureID>(static_cast<uintptr_t>(TextureID)), Available, ImVec2(0, 1), ImVec2(1, 0));
    else
      ImGui::TextUnformatted("Render texture unavailable");
  }

  ImGui::End();
}

} // namespace editor

#endif