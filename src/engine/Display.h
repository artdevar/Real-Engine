#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include "engine/MathCore.h"

class CDisplay
{
public:
  using ResizeCallback      = std::function<void(int, int)>;
  using MouseButtonCallback = std::function<void(int, int, int)>;
  using MouseScrollCallback = std::function<void(float, float)>;
  using MouseMoveCallback   = std::function<void(float, float)>;
  using KeyCallback         = std::function<void(int, int, int)>;

public:
  CDisplay();
  ~CDisplay();

  int Init(const std::string &_Title);
  void Shutdown();

  void PollEvents();
  void SwapBuffers();
  bool ShouldClose() const;
  void SetShouldClose(bool _ShouldClose);

  void SetTitle(const std::string &_Title);
  TVector2i GetSize() const;
  GLFWwindow *GetWindow() const;

  void SetCursorMode(int _Mode);
  void GetCursorPos(double &_X, double &_Y) const;
  void SetCursorPos(double _X, double _Y);

  // Callbacks
  void SetResizeCallback(ResizeCallback _Callback);
  void SetMouseButtonCallback(MouseButtonCallback _Callback);
  void SetMouseScrollCallback(MouseScrollCallback _Callback);
  void SetMouseMoveCallback(MouseMoveCallback _Callback);
  void SetKeyCallback(KeyCallback _Callback);

private:
  void InitCallbacks();

  static void OnWindowResizedProxy(GLFWwindow *_Window, int _Width, int _Height);
  static void OnMousePressedProxy(GLFWwindow *_Window, int _Button, int _Action, int _Mods);
  static void OnMouseScrollProxy(GLFWwindow *_Window, double _OffsetX, double _OffsetY);
  static void OnMouseMoveProxy(GLFWwindow *_Window, double _PosX, double _PosY);
  static void OnKeyPressedProxy(GLFWwindow *_Window, int _Key, int _Scancode, int _Action, int _Mods);
  static void OnErrorOccuredProxy(int _ErrorCode, const char *_Description);

private:
  static constexpr int DEFAULT_WIDTH  = 1400;
  static constexpr int DEFAULT_HEIGHT = 900;

private:
  GLFWwindow *m_Window;

  ResizeCallback      m_ResizeCallback;
  MouseButtonCallback m_MouseButtonCallback;
  MouseScrollCallback m_MouseScrollCallback;
  MouseMoveCallback   m_MouseMoveCallback;
  KeyCallback         m_KeyCallback;
};