#pragma once

#include <glm/vec2.hpp>
#include <memory>
#include <vector>
#include <string>
#include "Defines.h"
#include "interfaces/Shutdownable.h"
#include "interfaces/Updateable.h"
#include "interfaces/Renderable.h"
#include "utils/Common.h"

class CWorld;
class CEditorUI;
class CShader;
class CCamera;
class CResourceManager;
class CModel;
class CDisplay;
class CInputManager;

class CEngine final : public IShutdownable,
                      private IUpdateable,
                      private IRenderable
{
  DISABLE_CLASS_COPY(CEngine);

protected:
  CEngine();

  ~CEngine();

public:
  static CEngine &Instance();

  void Shutdown();

  int Init(const std::string &_ConfigPath, const std::string &_GameTitle);
  int Run();

  void LoadConfig();
  void SaveConfig();

public:
  glm::ivec2 GetWindowSize() const;

  CDisplay *GetDisplay() const;
  std::shared_ptr<CWorld> GetWorld() const;
  std::shared_ptr<CCamera> GetCamera() const;
  std::shared_ptr<CResourceManager> GetResourceManager() const;
  std::shared_ptr<CInputManager> GetInputManager() const;

private:
  void UpdateInternal(float _TimeDelta) override;
  bool ShouldBeUpdated() const override;
  void RenderInternal(CRenderer &_Renderer) override;
  bool ShouldBeRendered() const override;

  void OnWindowResized(int _Width, int _Height);
  void ProcessInput(float _TimeDelta);
  void DispatchKeyInput(int _Key, int _Action, int _Mods);
  void DispatchMouseButton(int _Button, int _Action, int _Mods);
  void DispatchMouseMove(float _X, float _Y);

private:
  static CEngine *Singleton;

#if EDITOR_ENABLED
  bool m_CameraDragActive{false};
#endif

private:
  std::unique_ptr<CDisplay> m_Display;
  std::shared_ptr<CInputManager> m_InputManager;
  std::shared_ptr<CCamera> m_Camera;
  std::shared_ptr<CWorld> m_World;
  std::shared_ptr<CResourceManager> m_ResourceManager;

#if EDITOR_ENABLED
  CEditorUI *m_EditorUI;
#endif
};