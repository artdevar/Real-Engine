#pragma once

#if DEV_STAGE
#include <events/EventsListener.h>
#include <common/interfaces/Shutdownable.h>
#include <common/Sharable.h>
#include <common/MathTypes.h>
#include <memory>

class IWorldEditor;
struct GLFWwindow;

namespace editor
{

class CMenuBar;
class CEntitiesWindow;
class CComponentDataWindow;
class CGlobalParamsWindow;
class CComponentRenderer;
class CViewportWindow;
class CPerformanceWindow;

class CEditorUI : public CSharable<CEditorUI>,
                  public IEventsListener,
                  public IShutdownable
{
public:
  CEditorUI(IWorldEditor &_WorldEditor);
  ~CEditorUI();

  void Shutdown() override;
  void OnEvent(const TEvent &_Event) override;

  void Init(GLFWwindow *_Window);

  TVector2i GetViewportSize() const;

  void RenderFrame();

private:
  void RenderBegin();
  void RenderEnd();

  void SubscribeToEvents();

private:
  IWorldEditor &m_WorldEditor;

  std::unique_ptr<CComponentRenderer>   m_ComponentRenderer;
  std::unique_ptr<CComponentDataWindow> m_ComponentDataWindow;
  std::unique_ptr<CGlobalParamsWindow>  m_GlobalParamsWindow;
  std::unique_ptr<CViewportWindow>      m_ViewportWindow;
  std::unique_ptr<CMenuBar>             m_MenuBar;
  std::unique_ptr<CEntitiesWindow>      m_EntitiesWindow;
  std::unique_ptr<CPerformanceWindow>   m_PerformanceWindow;
};

} // namespace editor
#endif