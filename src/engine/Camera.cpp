#include "Camera.h"
#include "Engine.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

CCamera::CCamera() :
  m_Position(0.0f, 0.0f, 8.0f),
  m_Forward (0.0f, 0.0f, -1.0f),
  m_Up      (0.0f, 1.0f, 0.0f),
  m_FOV     (45.0f),
  m_Yaw     (-90.0f),
  m_Pitch   (0.0f)
{
  // Empty
}

void CCamera::Update(float _TimeDelta)
{
  if (!m_PressedKeys.empty())
  {
    const bool  IsShiftPressed  = m_PressedKeys.contains(GLFW_KEY_LEFT_SHIFT);
    const float CameraSpeedMult = 0.005f * _TimeDelta * (IsShiftPressed ? 4.0f : 1.0f);

    for (int Key : m_PressedKeys)
    {
      switch (Key)
      {
        case GLFW_KEY_W:
          m_Position += CameraSpeedMult * m_Forward;
          break;

        case GLFW_KEY_S:
          m_Position -= CameraSpeedMult * m_Forward;
          break;

        case GLFW_KEY_A:
          m_Position -= glm::normalize(glm::cross(m_Forward, m_Up)) * CameraSpeedMult;
          break;

        case GLFW_KEY_D:
          m_Position += glm::normalize(glm::cross(m_Forward, m_Up)) * CameraSpeedMult;
          break;
      }
    }
  }
}

bool CCamera::OnMousePressed(int _Button, int _Action, int _Mods)
{
  if (_Button != GLFW_MOUSE_BUTTON_RIGHT)
    return false;

  if (_Action == GLFW_RELEASE)
  {
    m_PressedKeys.erase(GLFW_MOUSE_BUTTON_RIGHT);
    m_FOV = 45.0f;
  }
  else
  {
    m_PressedKeys.insert(GLFW_MOUSE_BUTTON_RIGHT);
    m_FOV = 20.0f;
  }

  return true;
}

bool CCamera::ProcessKeyInput(int _Key, int _Action, int _Mods)
{
  if (_Action == GLFW_RELEASE)
  {
    const auto Iter    = m_PressedKeys.find(_Key);
    const bool Handled = (Iter != m_PressedKeys.end());

    if (Iter != m_PressedKeys.end())
      m_PressedKeys.erase(Iter);

    return Handled;
  }

  switch (_Key)
  {
    case GLFW_KEY_W:
    case GLFW_KEY_S:
    case GLFW_KEY_A:
    case GLFW_KEY_D:
    case GLFW_KEY_LEFT_SHIFT:
      m_PressedKeys.insert(_Key);
      return true;

    default:
      return false;
  }
}

bool CCamera::ProcessMouseMove(float _X, float _Y)
{
  if (!m_PrevMousePos.has_value())
    m_PrevMousePos.emplace(_X, _Y);

  float OffsetX = _X - m_PrevMousePos->x;
  float OffsetY = m_PrevMousePos->y - _Y;

  m_PrevMousePos.emplace(_X, _Y);

  const float Sensitivity = 0.1f;
  OffsetX *= Sensitivity;
  OffsetY *= Sensitivity;

  m_Yaw  += OffsetX;
  m_Pitch = std::max(-89.0f, std::min(89.0f, m_Pitch + OffsetY));

  const glm::vec3 Direction(
      cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch)),
      sin(glm::radians(m_Pitch)),
      sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch))
    );

  m_Forward = glm::normalize(Direction);
  return true;
}

void CCamera::SetPosition(const glm::vec3 & _Pos)
{
  m_Position = _Pos;
}

glm::vec3 CCamera::GetPosition() const
{
  return m_Position;
}

glm::vec3 CCamera::GetForwardVector() const
{
  return m_Forward;
}

glm::vec3 CCamera::GetUpVector() const
{
  return m_Up;
}

glm::mat4 CCamera::GetView() const
{
  return glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
}

glm::mat4 CCamera::GetProjection() const
{
  const glm::ivec2 WindowSize = CEngine::Instance()->GetWindowSize();
  return glm::perspective(glm::radians(GetFOV()), WindowSize.x / float(WindowSize.y), 0.1f, 100.0f);
}

float CCamera::GetFOV() const
{
  return m_FOV;
}