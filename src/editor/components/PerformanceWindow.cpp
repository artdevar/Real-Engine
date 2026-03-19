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
  static const RenderPassesList RenderPasses = {std::make_pair(ERenderPassType::Common_Utility, "Utility"),          //
                                                std::make_pair(ERenderPassType::Common_Shadow, "Shadow"),            //
                                                std::make_pair(ERenderPassType::Common_Geometry, "Geometry"),        //
                                                std::make_pair(ERenderPassType::Common_PostProcess, "Post Process"), //
                                                std::make_pair(ERenderPassType::Common_Debug, "Debug"),              //
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
      float MinFpsLimit = m_FPSHistory.Min - 10;
      float MaxFpsLimit = m_FPSHistory.Max + 10;

      if (m_IsTargetFPSSet)
      {
        MinFpsLimit = std::min(MinFpsLimit, m_TargetFPS - 10.0f);
        MaxFpsLimit = std::max(MaxFpsLimit, m_TargetFPS + 10.0f);
      }

      ImPlot::SetupAxisLimits(ImAxis_X1, m_FPSHistory.History.front().X, m_FPSHistory.History.back().X, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, MinFpsLimit, MaxFpsLimit, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y2, m_FrameTimeHistory.Min - 1.0f, m_FrameTimeHistory.Max + 1.0f, ImGuiCond_Always);
    }

    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);

    ImPlot::SetAxis(ImAxis_Y1);
    if (!m_FPSHistory.History.empty())
      ImPlot::PlotLineG("FPS", GetPlotPoint, &m_FPSHistory, m_FPSHistory.History.size(), ImPlotLineFlags_None);

    if (m_IsTargetFPSSet)
    {
      const float TargetValues[] = {static_cast<float>(m_TargetFPS), static_cast<float>(m_TargetFPS)};
      const float TargetX[]      = {m_FPSHistory.History.front().X, m_FPSHistory.History.back().X};
      ImPlot::PlotLine("Target FPS", TargetX, TargetValues, 2);
    }

    ImPlot::SetAxis(ImAxis_Y2);
    if (!m_FrameTimeHistory.History.empty())
      ImPlot::PlotLineG("Tick (ms)", GetPlotPoint, &m_FrameTimeHistory, m_FrameTimeHistory.History.size(), ImPlotLineFlags_None);

    ImPlot::PopStyleVar();
    ImPlot::EndPlot();
  }

  if (ImGui::CollapsingHeader("Details##FPSDetails"))
  {
    ImGui::Columns(2, "PerformanceStats");
    ImGui::Text("FPS");
    ImGui::NextColumn();
    ImGui::Text("Tick (ms)");
    ImGui::Separator();
    ImGui::NextColumn();
    ImGui::Text("Min: %.1f", m_FPSHistory.Min);
    ImGui::Text("Max: %.1f", m_FPSHistory.Max);
    ImGui::Text("Avg: %.1f", m_FPSHistory.Avg);
    ImGui::NextColumn();
    ImGui::Text("Min: %.2f", m_FrameTimeHistory.Min);
    ImGui::Text("Max: %.2f", m_FrameTimeHistory.Max);
    ImGui::Text("Avg: %.2f", m_FrameTimeHistory.Avg);
    ImGui::Columns(1);
  }

  if (ImGui::CollapsingHeader("Settings##FPSSettings"))
  {
    ImGui::Checkbox("Target FPS", &m_IsTargetFPSSet);
    if (m_IsTargetFPSSet)
    {
      ImGui::SameLine();
      ImGui::SliderInt("##TargetFPSSlider", &m_TargetFPS, 10, 1000);
    }
  }
}

void CPerformanceWindow::RenderPassesSection()
{
  if (!ImGui::CollapsingHeader("Render Passes"))
    return;

  const auto &RenderPasses = GetRenderPasses();
  RenderPassesPlot(RenderPasses);

  if (ImGui::CollapsingHeader("Details##RenderPassesDetails"))
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
      ImPlot::PlotLineG(_RenderPasses[i].second.data(),      //
                        GetPlotPoint,                        //
                        &m_RenderPassStats[i],               //
                        m_RenderPassStats[i].History.size(), //
                        ImPlotLineFlags_None);
    }
  }

  ImPlot::PopStyleVar();
  ImPlot::EndPlot();
}

void CPerformanceWindow::RenderPassesStatistics(const RenderPassesList &_RenderPasses)
{
  ImGui::Columns(4, "RenderPassStats");

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
    ImGui::Text("%.2f", m_RenderPassStats[i].Min);
    ImGui::NextColumn();
    ImGui::Text("%.2f", m_RenderPassStats[i].Max);
    ImGui::NextColumn();
    ImGui::Text("%.2f", m_RenderPassStats[i].Avg);
    ImGui::NextColumn();
  }

  ImGui::Columns(1);
}

void CPerformanceWindow::UpdateFPSHistory()
{
  constexpr auto Comparator = [](const TVector2f &a, const TVector2f &b) {
    return a.Y < b.Y;
  };

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

  m_FPSHistory.Min = std::min_element(m_FPSHistory.History.begin(), m_FPSHistory.History.end(), Comparator)->Y;
  m_FPSHistory.Max = std::max_element(m_FPSHistory.History.begin(), m_FPSHistory.History.end(), Comparator)->Y;

  m_FPSHistory.Avg = std::accumulate(m_FPSHistory.History.begin(), m_FPSHistory.History.end(), 0.0f,
                                     [](float sum, const TVector2f &point) {
                                       return sum + point.Y;
                                     }) /
                     m_FPSHistory.History.size();

  m_FrameTimeHistory.Min = std::min_element(m_FrameTimeHistory.History.begin(), m_FrameTimeHistory.History.end(), Comparator)->Y;
  m_FrameTimeHistory.Max = std::max_element(m_FrameTimeHistory.History.begin(), m_FrameTimeHistory.History.end(), Comparator)->Y;

  m_FrameTimeHistory.Avg = std::accumulate(m_FrameTimeHistory.History.begin(), m_FrameTimeHistory.History.end(), 0.0f,
                                           [](float sum, const TVector2f &point) {
                                             return sum + point.Y;
                                           }) /
                           m_FrameTimeHistory.History.size();
}

void CPerformanceWindow::UpdateRenderPassHistory()
{
  constexpr auto Comparator = [](const TVector2f &a, const TVector2f &b) {
    return a.Y < b.Y;
  };

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

    Stats.Min = std::min_element(Stats.History.begin(), Stats.History.end(), Comparator)->Y;
    Stats.Max = std::max_element(Stats.History.begin(), Stats.History.end(), Comparator)->Y;

    Stats.Avg = std::accumulate(Stats.History.begin(), Stats.History.end(), 0.0f,
                                [](float sum, const TVector2f &point) {
                                  return sum + point.Y;
                                }) /
                Stats.History.size();

    m_MaxRenderPassTime = std::max(m_MaxRenderPassTime, Stats.Max);
  }
}

} // namespace editor

#endif
