#if DEV_STAGE

#include "OverviewWindow.h"
#include "engine/Engine.h"
#include "interfaces/RenderPipeline.h"
#include <imgui/imgui.h>

namespace
{

std::string FormatCount(int _Count)
{
  if (_Count >= 1'000'000)
    return std::format("{:.1f}M", static_cast<float>(_Count) / 1'000'000.0f);
  if (_Count >= 1'000)
    return std::format("{:.1f}K", static_cast<float>(_Count) / 1'000.0f);
  return std::to_string(_Count);
}

} // namespace

namespace editor
{

std::string COverviewWindow::GetName() const
{
  return "Overview";
}

TVector2i COverviewWindow::GetSize() const
{
  return m_Size;
}

void COverviewWindow::Render()
{
  if (ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
  {
    const ImVec2    Available = ImGui::GetContentRegionAvail();
    const TVector2i NewSize   = TVector2i(Available.x, Available.y);

    m_Size = NewSize;

    auto &Engine = CEngine::Instance();

    if (ImGui::CollapsingHeader("Render data"))
    {
      if (const auto Pipeline = Engine.GetRenderPipeline())
      {
        ImGui::Text("Draw calls: %s", FormatCount(Pipeline->GetDrawCallsCount()).c_str());
        ImGui::Text("Vertices submitted: %s", FormatCount(Pipeline->GetVerticesCount()).c_str());
        ImGui::Text("Indices submitted: %s", FormatCount(Pipeline->GetIndicesCount()).c_str());
        ImGui::Text("Triangles: %s", FormatCount(Pipeline->GetTrianglesCount()).c_str());
        ImGui::Text("Lines: %s", FormatCount(Pipeline->GetLinesCount()).c_str());
        ImGui::Text("Points: %s", FormatCount(Pipeline->GetPointsCount()).c_str());
      }
    }
  }

  ImGui::End();
}

} // namespace editor

#endif