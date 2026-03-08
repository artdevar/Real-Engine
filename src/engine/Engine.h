#pragma once

#include "interfaces/Renderer.h"
#include "interfaces/Shutdownable.h"
#include "interfaces/Updateable.h"
#include "interfaces/EventsListener.h"
#include <common/Sharable.h>
#include <common/MathTypes.h>
#include <common/Common.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class CWorld;
class CShader;
class CCamera;
class CResourceManager;
class CModel;
class CDisplay;
class CInputManager;
class CEventsManager;
class IRenderPipeline;

namespace editor
{
class CEditorUI;
}

class CEngine final : public CSharable<CEngine>,
                      public IEventsListener,
                      public IShutdownable,
                      private IUpdateable
{
  DISABLE_CLASS_COPY(CEngine);
  using CSharable<CEngine>::Create;

public:
  static CEngine &Instance();

  CEngine();
  ~CEngine();

  void Shutdown();

  int Init();
  int Run();

public:
  TVector2i GetWindowSize() const;
  TVector2i GetViewportSize() const;
  uint32_t GetRenderTextureID() const;

  CDisplay *GetDisplay() const;
  std::shared_ptr<CWorld> GetWorld() const;
  std::shared_ptr<CCamera> GetCamera() const;
  std::shared_ptr<CResourceManager> GetResourceManager() const;
  std::shared_ptr<CInputManager> GetInputManager() const;
  std::shared_ptr<CEventsManager> GetEventsManager() const;
  std::shared_ptr<IRenderPipeline> GetRenderPipeline() const;

  float GetFrameTime() const;
  int GetFPS() const;
  float GetApplicationRunningTime() const;

  void OnEvent(const TEvent &_Event) override;

private:
  void Render(IRenderer &_Renderer);
  void Update(float _TimeDelta) override;

  void SetFrameTime(float _Time);

  void SubscribeToEvents();
  void SubscribeToCallbacks();

  void OnWindowResized(int _Width, int _Height);
  void ProcessInput(float _TimeDelta);
  void DispatchKeyInput(int _Key, int _Action, int _Mods);
  void DispatchMouseButton(int _Button, int _Action, int _Mods);
  void DispatchMouseMove(float _X, float _Y);

private:
  static TSharedPtr Singleton;

#if DEV_STAGE
  bool m_CameraDragActive{false};
#endif

private:
  float m_FrameTime;
  bool  m_RequestShutdown;

  std::unique_ptr<CDisplay>         m_Display;
  std::shared_ptr<CInputManager>    m_InputManager;
  std::shared_ptr<CEventsManager>   m_EventsManager;
  std::shared_ptr<CCamera>          m_Camera;
  std::shared_ptr<CWorld>           m_World;
  std::shared_ptr<CResourceManager> m_ResourceManager;
  std::shared_ptr<IRenderPipeline>  m_RenderPipeline;

#if DEV_STAGE
  std::shared_ptr<editor::CEditorUI> m_EditorUI;
#endif
};