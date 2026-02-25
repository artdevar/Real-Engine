#include "pch.h"

#include "Camera.h"
#include "Config.h"
#include "Engine.h"
#include "events/Event.h"
#include "utils/Event.h"
#include <GLFW/glfw3.h>

CCamera::CCamera() :
    m_Position(0.0f, 0.0f, 8.0f),
    m_Forward(0.0f, 0.0f, -1.0f),
    m_Up(0.0f, 1.0f, 0.0f),
    m_Yaw(-90.0f),
    m_Pitch(0.0f),
    m_FOV(60.0f),
    m_ZNear(0.1f),
    m_ZFar(100.0f)
{
}

void CCamera::Init()
{
  m_FOV   = CConfig::Instance().GetCameraFOV();
  m_ZNear = CConfig::Instance().GetCameraZNear();
  m_ZFar  = CConfig::Instance().GetCameraZFar();

  event::Subscribe(TEventType::Config_CameraFOVChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_CameraZNearChanged, GetWeakPtr());
  event::Subscribe(TEventType::Config_CameraZFarChanged, GetWeakPtr());
}

void CCamera::Shutdown()
{
  event::Unsubscribe(TEventType::Config_CameraFOVChanged, GetWeakPtr());
  event::Unsubscribe(TEventType::Config_CameraZNearChanged, GetWeakPtr());
  event::Unsubscribe(TEventType::Config_CameraZFarChanged, GetWeakPtr());
}

void CCamera::UpdateInternal(float _TimeDelta)
{
  glm::vec3 movementDirection(0.0f);

  if (m_MoveForward)
    movementDirection += m_Forward;
  if (m_MoveBackward)
    movementDirection -= m_Forward;
  if (m_MoveLeft)
    movementDirection -= glm::normalize(glm::cross(m_Forward, m_Up));
  if (m_MoveRight)
    movementDirection += glm::normalize(glm::cross(m_Forward, m_Up));

  if (glm::length(movementDirection) > 0.0f)
  {
    const float CameraSpeedMult = 0.005f * _TimeDelta * m_SpeedMultiplier;
    m_Position                 += glm::normalize(movementDirection) * CameraSpeedMult;
  }

  if (m_MouseDelta.x != 0.0f || m_MouseDelta.y != 0.0f)
  {
    const float Sensitivity = 0.1f;

    m_Yaw  += m_MouseDelta.x * Sensitivity;
    m_Pitch = std::max(-89.0f, std::min(89.0f, m_Pitch + m_MouseDelta.y * Sensitivity));

    const glm::vec3 Direction(cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch)), sin(glm::radians(m_Pitch)),
                              sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch)));

    m_Forward = glm::normalize(Direction);
  }

  m_MouseDelta = glm::vec2(0.0f, 0.0f);
}

void CCamera::SetPosition(const glm::vec3 &_Pos)
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
  const TVector2i WindowSize = CEngine::Instance().GetWindowSize();
  return glm::perspective(glm::radians(m_FOV), WindowSize.X / float(WindowSize.Y), m_ZNear, m_ZFar);
}

void CCamera::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::Config_CameraFOVChanged:
    m_FOV = _Event.GetValue<float>();
    break;
  case TEventType::Config_CameraZNearChanged:
    m_ZNear = _Event.GetValue<float>();
    break;
  case TEventType::Config_CameraZFarChanged:
    m_ZFar = _Event.GetValue<float>();
    break;

  default:
    break;
  }
}

bool CCamera::OnMousePressed(int _Button, int _Action, int _Mods)
{
  return false;
}

bool CCamera::ProcessKeyInput(int _Key, int _Action, int _Mods)
{
  if (_Action == GLFW_PRESS)
  {
    switch (_Key)
    {
    case GLFW_KEY_W:
      m_MoveForward = true;
      return true;
    case GLFW_KEY_S:
      m_MoveBackward = true;
      return true;
    case GLFW_KEY_A:
      m_MoveLeft = true;
      return true;
    case GLFW_KEY_D:
      m_MoveRight = true;
      return true;
    case GLFW_KEY_LEFT_SHIFT:
      m_SpeedMultiplier = 4.0f;
      return true;
    default:
      return false;
    }
  }
  else if (_Action == GLFW_RELEASE)
  {
    switch (_Key)
    {
    case GLFW_KEY_W:
      m_MoveForward = false;
      return true;
    case GLFW_KEY_S:
      m_MoveBackward = false;
      return true;
    case GLFW_KEY_A:
      m_MoveLeft = false;
      return true;
    case GLFW_KEY_D:
      m_MoveRight = false;
      return true;
    case GLFW_KEY_LEFT_SHIFT:
      m_SpeedMultiplier = 1.0f;
      return true;
    default:
      return false;
    }
  }

  return false;
}

bool CCamera::ProcessMouseMove(float _X, float _Y)
{
  m_MouseDelta.x += _X;
  m_MouseDelta.y += _Y;
  return true;
}

void CCamera::ResetInputState()
{
  m_MoveForward     = false;
  m_MoveBackward    = false;
  m_MoveLeft        = false;
  m_MoveRight       = false;
  m_SpeedMultiplier = 1.0f;
  m_MouseDelta      = glm::vec2(0.0f, 0.0f);
}

bool CCamera::ShouldBeUpdated() const
{
  return true;
}