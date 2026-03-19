#pragma once

#if DEV_STAGE

#include "EditorWindow.h"
#include <deque>
#include <vector>
#include <array>

enum class ERenderPassType;

namespace editor
{

struct THistoryBuffer
{
  std::deque<TVector2f> History;
  float                 Min = 0;
  float                 Max = 0;
  float                 Avg = 0;
};

class CPerformanceWindow : public IEditorWindow
{
public:
  std::string GetName() const override;
  TVector2i GetSize() const override;

  void Render();

private:
  using RenderPassesList = std::vector<std::pair<ERenderPassType, std::string_view>>;

  void UpdateFPSHistory();
  void UpdateRenderPassHistory();

  void RenderFPSSection();
  void RenderPassesSection();
  void RenderPassesPlot(const RenderPassesList &_RenderPasses);
  void RenderPassesStatistics(const RenderPassesList &_RenderPasses);

  static const RenderPassesList &GetRenderPasses();

private:
  constexpr static size_t MAX_HISTORY       = 1000;
  constexpr static size_t RENDER_PASS_COUNT = 6;

  TVector2i m_Size;
  int       m_TargetFPS      = 240;
  bool      m_IsTargetFPSSet = false;

  THistoryBuffer m_FPSHistory;
  THistoryBuffer m_FrameTimeHistory;

  std::array<THistoryBuffer, RENDER_PASS_COUNT> m_RenderPassStats;
  float                                         m_MaxRenderPassTime = 0;
};

} // namespace editor

#endif
