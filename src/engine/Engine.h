#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <memory>
#include <vector>
#include <string>
#include "interfaces/Shutdownable.h"

class CWorld;
class CEditorUI;
class CShader;
class CCamera;
class CResourceManager;
class CModel;

class CEngine final :
  public IShutdownable
{
protected:

  CEngine();

  ~CEngine();

public:

  static CEngine * Instance();

  void Shutdown();

  int Init();

  int Run();

  void LoadConfig();

  void SaveConfig();

public:

  glm::ivec2 GetWindowSize() const;

  CWorld * GetWorld() const;

  std::shared_ptr<CCamera> GetCamera() const;

  std::shared_ptr<CResourceManager> GetResourceManager() const;

private:

  void InitCallbacks();

private: // Callbacks

  void OnWindowResized(int _Width, int _Height);

  void OnMousePressed(int _Button, int _Action, int _Mods);

  void OnMouseScroll(float _OffsetX, float _OffsetY);

  void ProcessMouseMove(float _X, float _Y);

  void ProcessKeyInput(int _Key, int _Action, int _Mods);

  void EnableDebugMode(bool _Enable);

  void OnGLErrorOccured(GLenum _Error);

private:

  static constexpr int WindowWidth  = 1200;
  static constexpr int WindowHeight = 800;

  static const std::string ConfigPath;
  static CEngine * Singleton;

public:

  GLFWwindow * m_Window;
  std::shared_ptr<CCamera> m_Camera;

  CWorld *    m_World;
  CEditorUI * m_EditorUI;

  std::shared_ptr<CResourceManager> m_ResourceManager;

  bool   m_IsEditorMode;
  double m_PrevCursorPosX, m_PrevCursorPosY;


};