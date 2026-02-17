#pragma once

#include "utils/Common.h"
#include <glm/vec2.hpp>
#include <optional>
#include <unordered_set>

class CInputManager
{
  DISABLE_CLASS_COPY(CInputManager);

public:
  CInputManager();
  ~CInputManager() = default;

  void Update();

  // Key state queries
  bool IsKeyPressed(int _Key) const;
  bool IsKeyJustPressed(int _Key) const;
  bool IsKeyJustReleased(int _Key) const;

  // Mouse button state queries
  bool IsMouseButtonPressed(int _Button) const;
  bool IsMouseButtonJustPressed(int _Button) const;
  bool IsMouseButtonJustReleased(int _Button) const;

  // Mouse movement
  glm::vec2 GetMousePosition() const;
  glm::vec2 GetMouseDelta() const;
  bool HasMouseMoved() const;

  // Mouse scroll
  glm::vec2 GetMouseScroll() const;

  // Input callbacks (called by Display)
  void OnKeyEvent(int _Key, int _Action, int _Mods);
  void OnMouseButton(int _Button, int _Action, int _Mods);
  void OnMouseMove(float _PosX, float _PosY);
  void OnMouseScroll(float _OffsetX, float _OffsetY);

private:
  std::unordered_set<int> m_PressedKeys;
  std::unordered_set<int> m_JustPressedKeys;
  std::unordered_set<int> m_JustReleasedKeys;

  std::unordered_set<int> m_PressedMouseButtons;
  std::unordered_set<int> m_JustPressedMouseButtons;
  std::unordered_set<int> m_JustReleasedMouseButtons;

  std::optional<glm::vec2> m_PreviousMousePos;
  glm::vec2                m_CurrentMousePos;
  glm::vec2                m_MouseDelta;

  glm::vec2 m_MouseScroll;
};
