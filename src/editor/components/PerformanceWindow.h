#pragma once

#if DEV_STAGE

#include "EditorWindow.h"
#include <vector>

namespace editor
{

class CPerformanceWindow : public IEditorWindow
{
public:
  CPerformanceWindow();

  std::string GetName() const override;
  TVector2i GetSize() const override;

  void Render();

private:
  void UpdateHistory(float _FPS, float _FrameTime, float _Time);

private:
  constexpr static size_t MAX_HISTORY = 1000;

  TVector2i m_Size;

  std::vector<TVector2f> m_FPSHistory;
  float                  m_MinFps = 0;
  float                  m_MaxFps = 0;
  float                  m_AvgFps = 0;

  std::vector<TVector2f> m_FrameTimeHistory;
  float                  m_MinFrameTime = 0;
  float                  m_MaxFrameTime = 0;
  float                  m_AvgFrameTime = 0;
};

} // namespace editor

#endif
