#include "InputManager.h"
#include <GLFW/glfw3.h>

CInputManager::CInputManager() : m_CurrentMousePos(0.0f, 0.0f), m_MouseDelta(0.0f, 0.0f), m_MouseScroll(0.0f, 0.0f)
{
}

void CInputManager::Update()
{
  m_JustPressedKeys.clear();
  m_JustReleasedKeys.clear();
  m_JustPressedMouseButtons.clear();
  m_JustReleasedMouseButtons.clear();

  m_MouseDelta  = glm::vec2(0.0f, 0.0f);
  m_MouseScroll = glm::vec2(0.0f, 0.0f);
}

bool CInputManager::IsKeyPressed(int _Key) const
{
  return m_PressedKeys.contains(_Key);
}

bool CInputManager::IsKeyJustPressed(int _Key) const
{
  return m_JustPressedKeys.contains(_Key);
}

bool CInputManager::IsKeyJustReleased(int _Key) const
{
  return m_JustReleasedKeys.contains(_Key);
}

bool CInputManager::IsMouseButtonPressed(int _Button) const
{
  return m_PressedMouseButtons.contains(_Button);
}

bool CInputManager::IsMouseButtonJustPressed(int _Button) const
{
  return m_JustPressedMouseButtons.contains(_Button);
}

bool CInputManager::IsMouseButtonJustReleased(int _Button) const
{
  return m_JustReleasedMouseButtons.contains(_Button);
}

glm::vec2 CInputManager::GetMousePosition() const
{
  return m_CurrentMousePos;
}

glm::vec2 CInputManager::GetMouseDelta() const
{
  return m_MouseDelta;
}

bool CInputManager::HasMouseMoved() const
{
  return m_MouseDelta.x != 0.0f || m_MouseDelta.y != 0.0f;
}

glm::vec2 CInputManager::GetMouseScroll() const
{
  return m_MouseScroll;
}

void CInputManager::OnKeyEvent(int _Key, int _Action, int _Mods)
{
  if (_Action == GLFW_PRESS || _Action == GLFW_REPEAT)
  {
    if (!m_PressedKeys.contains(_Key))
    {
      m_PressedKeys.insert(_Key);
      m_JustPressedKeys.insert(_Key);
    }
  }
  else if (_Action == GLFW_RELEASE)
  {
    m_PressedKeys.erase(_Key);
    m_JustReleasedKeys.insert(_Key);
  }
}

void CInputManager::OnMouseButton(int _Button, int _Action, int _Mods)
{
  if (_Action == GLFW_PRESS)
  {
    if (!m_PressedMouseButtons.contains(_Button))
    {
      m_PressedMouseButtons.insert(_Button);
      m_JustPressedMouseButtons.insert(_Button);
    }
  }
  else if (_Action == GLFW_RELEASE)
  {
    m_PressedMouseButtons.erase(_Button);
    m_JustReleasedMouseButtons.insert(_Button);
  }
}

void CInputManager::OnMouseMove(float _PosX, float _PosY)
{
  m_CurrentMousePos = glm::vec2(_PosX, _PosY);

  if (m_PreviousMousePos.has_value())
  {
    m_MouseDelta.x = _PosX - m_PreviousMousePos->x;
    m_MouseDelta.y = m_PreviousMousePos->y - _PosY; // Inverted Y
  }

  m_PreviousMousePos = m_CurrentMousePos;
}

void CInputManager::OnMouseScroll(float _OffsetX, float _OffsetY)
{
  m_MouseScroll = glm::vec2(_OffsetX, _OffsetY);
}
