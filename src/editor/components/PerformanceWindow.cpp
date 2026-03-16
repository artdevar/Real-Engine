#if DEV_STAGE

#include "PerformanceWindow.h"
#include "engine/Engine.h"
#include <imgui/imgui.h>
#include <imgui/implot/implot.h>
#include <algorithm>
#include <numeric>

namespace editor
{

CPerformanceWindow::CPerformanceWindow()
{
  m_FPSHistory.reserve(MAX_HISTORY);
  m_FrameTimeHistory.reserve(MAX_HISTORY);
}

std::string CPerformanceWindow::GetName() const
{
  return "Performance";
}

TVector2i CPerformanceWindow::GetSize() const
{
  return m_Size;
}

void CPerformanceWindow::Render()
{
  if (ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
  {
    const ImVec2    Available = ImGui::GetContentRegionAvail();
    const TVector2i NewSize   = TVector2i(Available.x, Available.y);

    m_Size = NewSize;

    auto &Engine = CEngine::Instance();
    UpdateHistory(Engine.GetFPS(), Engine.GetFrameTime(), Engine.GetApplicationRunningTime());

    if (ImGui::CollapsingHeader("FPS", ImGuiTreeNodeFlags_DefaultOpen))
    {
      if (ImPlot::BeginPlot("##PerformancePlot", ImVec2(-1, ImGui::GetTextLineHeight() * 12)))
      {
        ImPlot::SetupAxes(nullptr, nullptr, 0, ImPlotAxisFlags_RangeFit);
        ImPlot::SetupAxis(ImAxis_Y2, nullptr, ImPlotAxisFlags_AuxDefault | ImPlotAxisFlags_RangeFit);
        ImPlot::SetupLegend(ImPlotLocation_North, ImPlotLegendFlags_Horizontal);

        ImPlot::SetupAxisLimits(ImAxis_X1, m_FPSHistory.front().X, m_FPSHistory.back().X, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, m_MinFps - 10.0f, m_MaxFps + 10.0f, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y2, m_MinFrameTime - 1.0f, m_MaxFrameTime + 1.0f, ImGuiCond_Always);

        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);
        ImPlot::SetAxis(ImAxis_Y1);
        ImPlot::PlotLine("FPS", &m_FPSHistory.front().X, &m_FPSHistory.front().Y, (int)m_FPSHistory.size(), 0, 0, sizeof(TVector2f));

        ImPlot::SetAxis(ImAxis_Y2);
        ImPlot::PlotLine("Latency (ms)", &m_FrameTimeHistory.front().X, &m_FrameTimeHistory.front().Y, (int)m_FrameTimeHistory.size(), 0, 0,
                         sizeof(TVector2f));
        ImPlot::PopStyleVar();

        ImPlot::EndPlot();
      }

      ImGui::Columns(2, "PerfStats");
      ImGui::Text("FPS");
      ImGui::Text("Min: %.1f", m_MinFps);
      ImGui::Text("Max: %.1f", m_MaxFps);
      ImGui::Text("Avg: %.1f", m_AvgFps);
      ImGui::NextColumn();
      ImGui::Text("Latency (ms)");
      ImGui::Text("Min: %.2f", m_MinFrameTime);
      ImGui::Text("Max: %.2f", m_MaxFrameTime);
      ImGui::Text("Avg: %.2f", m_AvgFrameTime);
      ImGui::Columns(1);
    }
  }

  ImGui::End();
}

void CPerformanceWindow::UpdateHistory(float _FPS, float _FrameTime, float _Time)
{
  constexpr float Smoothing         = 0.9f;
  static float    SmoothedFps       = 0.0f;
  static float    SmoothedFrameTime = 0.0f;

  SmoothedFps       = SmoothedFps * Smoothing + _FPS * (1.0f - Smoothing);
  SmoothedFrameTime = SmoothedFrameTime * Smoothing + _FrameTime * (1.0f - Smoothing);

  if (m_FPSHistory.size() == m_FPSHistory.capacity())
    m_FPSHistory.erase(m_FPSHistory.begin());

  if (m_FrameTimeHistory.size() == m_FrameTimeHistory.capacity())
    m_FrameTimeHistory.erase(m_FrameTimeHistory.begin());

  m_FPSHistory.emplace_back(_Time, SmoothedFps);
  m_FrameTimeHistory.emplace_back(_Time, SmoothedFrameTime);

  m_MinFps = std::min_element(m_FPSHistory.begin(), m_FPSHistory.end(), [](const TVector2f &a, const TVector2f &b) {
               return a.Y < b.Y;
             })->Y;

  m_MaxFps = std::max_element(m_FPSHistory.begin(), m_FPSHistory.end(), [](const TVector2f &a, const TVector2f &b) {
               return a.Y < b.Y;
             })->Y;

  m_AvgFps = std::accumulate(m_FPSHistory.begin(), m_FPSHistory.end(), 0.0f,
                             [](float sum, const TVector2f &point) {
                               return sum + point.Y;
                             }) /
             m_FPSHistory.size();

  m_MinFrameTime = std::min_element(m_FrameTimeHistory.begin(), m_FrameTimeHistory.end(), [](const TVector2f &a, const TVector2f &b) {
                     return a.Y < b.Y;
                   })->Y;

  m_MaxFrameTime = std::max_element(m_FrameTimeHistory.begin(), m_FrameTimeHistory.end(), [](const TVector2f &a, const TVector2f &b) {
                     return a.Y < b.Y;
                   })->Y;

  m_AvgFrameTime = std::accumulate(m_FrameTimeHistory.begin(), m_FrameTimeHistory.end(), 0.0f,
                                   [](float sum, const TVector2f &point) {
                                     return sum + point.Y;
                                   }) /
                   m_FrameTimeHistory.size();
}

} // namespace editor

#endif
