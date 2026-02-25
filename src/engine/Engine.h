#pragma once

#include "Defines.h"
#include "MathCore.h"
#include "interfaces/Renderer.h"
#include "interfaces/Shutdownable.h"
#include "interfaces/Updateable.h"
#include "utils/Common.h"
#include <memory>
#include <string>
#include <vector>

class CWorld;
class CEditorUI;
class CShader;
class CCamera;
class CResourceManager;
class CModel;
class CDisplay;
class CInputManager;
class CEventsManager;
class CRenderPipeline;

class CEngine final : public IShutdownable,
                      private IUpdateable
{
  DISABLE_CLASS_COPY(CEngine);

protected:
  CEngine();

  ~CEngine();

public:
  static CEngine &Instance();

  void Shutdown();

  int Init();
  int Run();

  void LoadConfig();
  void SaveConfig();

public:
  TVector2i GetWindowSize() const;

  CDisplay *GetDisplay() const;
  std::shared_ptr<CWorld> GetWorld() const;
  std::shared_ptr<CCamera> GetCamera() const;
  std::shared_ptr<CResourceManager> GetResourceManager() const;
  std::shared_ptr<CInputManager> GetInputManager() const;
  std::shared_ptr<CEventsManager> GetEventsManager() const;

private:
  void Render(IRenderer &_Renderer);

  void UpdateInternal(float _TimeDelta) override;
  bool ShouldBeUpdated() const override;

  void OnWindowResized(int _Width, int _Height);
  void ProcessInput(float _TimeDelta);
  void DispatchKeyInput(int _Key, int _Action, int _Mods);
  void DispatchMouseButton(int _Button, int _Action, int _Mods);
  void DispatchMouseMove(float _X, float _Y);

private:
  static CEngine *Singleton;

#if DEV_STAGE
  bool m_CameraDragActive{false};
#endif

private:
  std::unique_ptr<CDisplay>         m_Display;
  std::shared_ptr<CInputManager>    m_InputManager;
  std::shared_ptr<CEventsManager>   m_EventsManager;
  std::shared_ptr<CCamera>          m_Camera;
  std::shared_ptr<CWorld>           m_World;
  std::shared_ptr<CResourceManager> m_ResourceManager;
  std::unique_ptr<CRenderPipeline>  m_RenderPipeline;

#if DEV_STAGE
  std::unique_ptr<CEditorUI> m_EditorUI;
#endif
};