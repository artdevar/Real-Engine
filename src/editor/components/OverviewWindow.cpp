#if DEV_STAGE

#include "OverviewWindow.h"
#include "engine/Engine.h"
#include "engine/Camera.h"
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

    if (ImGui::CollapsingHeader("Application"))
    {
      ImGui::Text("Window size: %d x %d", Engine.GetWindowSize().X, Engine.GetWindowSize().Y);
      ImGui::Text("Viewport size: %d x %d", Engine.GetViewportSize().X, Engine.GetViewportSize().Y);
      ImGui::Text("Running time: %.1f seconds", Engine.GetApplicationRunningTime());
    }

    if (ImGui::CollapsingHeader("Camera"))
    {
      if (const auto Camera = Engine.GetCamera())
      {
        const glm::vec3 CamPos = Camera->GetPosition();
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", CamPos.x, CamPos.y, CamPos.z);
      }
    }

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

        if (ImGui::CollapsingHeader("Shadow map"))
        {
          const ImVec2 ImageSize(Available.x, Available.x);
          ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(Pipeline->GetShadowMapTextureID())), ImageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
      }
    }
  }

  ImGui::End();
}

} // namespace editor

#endif