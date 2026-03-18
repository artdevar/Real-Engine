#if DEV_STAGE

#include "PerformanceWindow.h"
#include "engine/Engine.h"
#include "interfaces/RenderPipeline.h"
#include "render/passes/RenderPassTypes.h"
#include <imgui/imgui.h>
#include <imgui/implot/implot.h>
#include <algorithm>
#include <numeric>

namespace editor
{

static ImPlotPoint GetPlotPoint(int idx, void *user_data)
{
  THistoryBuffer *Data = static_cast<THistoryBuffer *>(user_data);
  return ImPlotPoint(Data->History[idx].X, Data->History[idx].Y);
}

const CPerformanceWindow::RenderPassesList &CPerformanceWindow::GetRenderPasses()
{
  static const RenderPassesList RenderPasses = {std::make_pair(ERenderPassType::Common_Utility, "Utility"),         //
                                                std::make_pair(ERenderPassType::Common_Shadow, "Shadow"),           //
                                                std::make_pair(ERenderPassType::Common_Geometry, "Geometry"),       //
                                                std::make_pair(ERenderPassType::Common_PostProcess, "PostProcess"), //
                                                std::make_pair(ERenderPassType::Common_Debug, "Debug"),             //
                                                std::make_pair(ERenderPassType::Common_Output, "Output")};

  return RenderPasses;
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
  if (!ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
  {
    ImGui::End();
    return;
  }

  const ImVec2 Available = ImGui::GetContentRegionAvail();
  m_Size                 = TVector2i(Available.x, Available.y);

  UpdateFPSHistory();
  UpdateRenderPassHistory();

  RenderFPSSection();
  RenderPassesSection();

  ImGui::End();
}

void CPerformanceWindow::RenderFPSSection()
{
  if (!ImGui::CollapsingHeader("FPS", ImGuiTreeNodeFlags_DefaultOpen))
    return;

  if (ImPlot::BeginPlot("##PerformancePlot", ImVec2(-1, ImGui::GetTextLineHeight() * 12)))
  {
    ImPlot::SetupAxes(nullptr, "FPS", ImPlotAxisFlags_RangeFit, ImPlotAxisFlags_RangeFit);
    ImPlot::SetupAxis(ImAxis_Y2, "Tick (ms)", ImPlotAxisFlags_AuxDefault | ImPlotAxisFlags_RangeFit);
    ImPlot::SetupLegend(ImPlotLocation_North, ImPlotLegendFlags_Horizontal);

    if (!m_FPSHistory.History.empty())
    {
      ImPlot::SetupAxisLimits(ImAxis_X1, m_FPSHistory.History.front().X, m_FPSHistory.History.back().X, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, m_FPSHistory.MinTime - 10.0f, m_FPSHistory.MaxTime + 10.0f, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y2, m_FrameTimeHistory.MinTime - 1.0f, m_FrameTimeHistory.MaxTime + 1.0f, ImGuiCond_Always);
    }

    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);

    ImPlot::SetAxis(ImAxis_Y1);
    if (!m_FPSHistory.History.empty())
      ImPlot::PlotLineG("FPS", GetPlotPoint, &m_FPSHistory, m_FPSHistory.History.size(), ImPlotLineFlags_None);

    ImPlot::SetAxis(ImAxis_Y2);
    if (!m_FrameTimeHistory.History.empty())
      ImPlot::PlotLineG("Tick (ms)", GetPlotPoint, &m_FrameTimeHistory, m_FrameTimeHistory.History.size(), ImPlotLineFlags_None);

    ImPlot::PopStyleVar();
    ImPlot::EndPlot();
  }

  ImGui::Columns(2, "PerfStats");
  ImGui::Text("FPS");
  ImGui::Text("Min: %.1f", m_FPSHistory.MinTime);
  ImGui::Text("Max: %.1f", m_FPSHistory.MaxTime);
  ImGui::Text("Avg: %.1f", m_FPSHistory.AvgTime);
  ImGui::NextColumn();
  ImGui::Text("Tick (ms)");
  ImGui::Text("Min: %.2f", m_FrameTimeHistory.MinTime);
  ImGui::Text("Max: %.2f", m_FrameTimeHistory.MaxTime);
  ImGui::Text("Avg: %.2f", m_FrameTimeHistory.AvgTime);
  ImGui::NextColumn();
  ImGui::Columns(1);
}

void CPerformanceWindow::RenderPassesSection()
{
  if (!ImGui::CollapsingHeader("Render Passes"))
    return;

  const auto &RenderPasses = GetRenderPasses();
  RenderPassesPlot(RenderPasses);

  ImGui::Spacing();

  RenderPassesStatistics(RenderPasses);
}

void CPerformanceWindow::RenderPassesPlot(const RenderPassesList &_RenderPasses)
{
  if (!ImPlot::BeginPlot("##RenderPassPlot", ImVec2(-1, ImGui::GetTextLineHeight() * 12)))
    return;

  ImPlot::SetupAxes(nullptr, "Time (ms)", ImPlotAxisFlags_RangeFit, ImPlotAxisFlags_RangeFit);
  ImPlot::SetupLegend(ImPlotLocation_North, ImPlotLegendFlags_Horizontal);

  if (!m_RenderPassStats[0].History.empty())
  {
    ImPlot::SetupAxisLimits(ImAxis_X1, m_RenderPassStats[0].History.front().X, m_RenderPassStats[0].History.back().X, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, m_MaxRenderPassTime + 1.0f, ImGuiCond_Always);
  }

  ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);

  for (size_t i = 0; i < _RenderPasses.size(); ++i)
  {
    if (!m_RenderPassStats[i].History.empty())
    {
      ImPlot::PlotLineG(_RenderPasses[i].second.data(), GetPlotPoint, &m_RenderPassStats[i], m_RenderPassStats[i].History.size(),
                        ImPlotLineFlags_None);
    }
  }

  ImPlot::PopStyleVar();
  ImPlot::EndPlot();
}

void CPerformanceWindow::RenderPassesStatistics(const RenderPassesList &_RenderPasses)
{
  if (!ImGui::CollapsingHeader("Render Pass Statistics"))
    return;

  ImGui::Columns(4, "RenderPassStats");
  ImGui::SetColumnWidth(0, 200);
  ImGui::SetColumnWidth(1, 80);
  ImGui::SetColumnWidth(2, 80);
  ImGui::SetColumnWidth(3, 80);

  ImGui::Text("Pass");
  ImGui::NextColumn();
  ImGui::Text("Min (ms)");
  ImGui::NextColumn();
  ImGui::Text("Max (ms)");
  ImGui::NextColumn();
  ImGui::Text("Avg (ms)");
  ImGui::NextColumn();
  ImGui::Separator();

  for (size_t i = 0; i < _RenderPasses.size(); ++i)
  {
    ImGui::Text("%s", _RenderPasses[i].second.data());
    ImGui::NextColumn();
    ImGui::Text("%.3f", m_RenderPassStats[i].MinTime);
    ImGui::NextColumn();
    ImGui::Text("%.3f", m_RenderPassStats[i].MaxTime);
    ImGui::NextColumn();
    ImGui::Text("%.3f", m_RenderPassStats[i].AvgTime);
    ImGui::NextColumn();
  }

  ImGui::Columns(1);
}

void CPerformanceWindow::UpdateFPSHistory()
{
  const float FPS         = CEngine::Instance().GetFPS();
  const float FrameTime   = CEngine::Instance().GetFrameTime();
  const float RunningTime = CEngine::Instance().GetApplicationRunningTime();

  constexpr float Smoothing         = 0.92f;
  static float    SmoothedFps       = 0.0f;
  static float    SmoothedFrameTime = 0.0f;

  SmoothedFps       = SmoothedFps * Smoothing + FPS * (1.0f - Smoothing);
  SmoothedFrameTime = SmoothedFrameTime * Smoothing + FrameTime * (1.0f - Smoothing);

  if (m_FPSHistory.History.size() == MAX_HISTORY)
    m_FPSHistory.History.pop_front();
  m_FPSHistory.History.emplace_back(RunningTime, SmoothedFps);

  if (m_FrameTimeHistory.History.size() == MAX_HISTORY)
    m_FrameTimeHistory.History.pop_front();
  m_FrameTimeHistory.History.emplace_back(RunningTime, SmoothedFrameTime);

  m_FPSHistory.MinTime = std::min_element(m_FPSHistory.History.begin(), m_FPSHistory.History.end(), [](const TVector2f &a, const TVector2f &b) {
                           return a.Y < b.Y;
                         })->Y;

  m_FPSHistory.MaxTime = std::max_element(m_FPSHistory.History.begin(), m_FPSHistory.History.end(), [](const TVector2f &a, const TVector2f &b) {
                           return a.Y < b.Y;
                         })->Y;

  m_FPSHistory.AvgTime = std::accumulate(m_FPSHistory.History.begin(), m_FPSHistory.History.end(), 0.0f,
                                         [](float sum, const TVector2f &point) {
                                           return sum + point.Y;
                                         }) /
                         m_FPSHistory.History.size();

  m_FrameTimeHistory.MinTime =
      std::min_element(m_FrameTimeHistory.History.begin(), m_FrameTimeHistory.History.end(), [](const TVector2f &a, const TVector2f &b) {
        return a.Y < b.Y;
      })->Y;

  m_FrameTimeHistory.MaxTime =
      std::max_element(m_FrameTimeHistory.History.begin(), m_FrameTimeHistory.History.end(), [](const TVector2f &a, const TVector2f &b) {
        return a.Y < b.Y;
      })->Y;

  m_FrameTimeHistory.AvgTime = std::accumulate(m_FrameTimeHistory.History.begin(), m_FrameTimeHistory.History.end(), 0.0f,
                                               [](float sum, const TVector2f &point) {
                                                 return sum + point.Y;
                                               }) /
                               m_FrameTimeHistory.History.size();
}

void CPerformanceWindow::UpdateRenderPassHistory()
{
  const auto  RenderPipeline = CEngine::Instance().GetRenderPipeline();
  const float RunningTime    = CEngine::Instance().GetApplicationRunningTime();

  const auto &RenderPasses = GetRenderPasses();

  constexpr float                             Smoothing         = 0.92f;
  static std::array<float, RENDER_PASS_COUNT> SmoothedPassTimes = {};

  for (size_t i = 0; i < RenderPasses.size(); ++i)
  {
    const float PassTime = RenderPipeline->GetRenderPassTime(RenderPasses[i].first);
    SmoothedPassTimes[i] = SmoothedPassTimes[i] * Smoothing + PassTime * (1.0f - Smoothing);

    auto &Stats = m_RenderPassStats[i];

    if (Stats.History.size() == MAX_HISTORY)
      Stats.History.pop_front();
    Stats.History.emplace_back(RunningTime, SmoothedPassTimes[i]);

    Stats.MinTime = std::min_element(Stats.History.begin(), Stats.History.end(), [](const TVector2f &a, const TVector2f &b) {
                      return a.Y < b.Y;
                    })->Y;

    Stats.MaxTime = std::max_element(Stats.History.begin(), Stats.History.end(), [](const TVector2f &a, const TVector2f &b) {
                      return a.Y < b.Y;
                    })->Y;

    Stats.AvgTime = std::accumulate(Stats.History.begin(), Stats.History.end(), 0.0f,
                                    [](float sum, const TVector2f &point) {
                                      return sum + point.Y;
                                    }) /
                    Stats.History.size();

    m_MaxRenderPassTime = std::max(m_MaxRenderPassTime, Stats.MaxTime);
  }
}

} // namespace editor

#endif
