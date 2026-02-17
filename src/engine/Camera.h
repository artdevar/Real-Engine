#pragma once

#include "interfaces/InputHandler.h"
#include "interfaces/Updateable.h"

class CCamera final : public IUpdateable,
                      public IInputHandler
{
public:
  CCamera();

  void SetPosition(const glm::vec3 &_Pos);

  glm::vec3 GetPosition() const;

  glm::vec3 GetForwardVector() const;

  glm::vec3 GetUpVector() const;

  glm::mat4 GetView() const;

  glm::mat4 GetProjection() const;

public: // Input
  bool OnMousePressed(int _Button, int _Action, int _Mods) override;

  bool ProcessKeyInput(int _Key, int _Action, int _Mods) override;

  bool ProcessMouseMove(float _X, float _Y) override;

  void ResetInputState();

private:
  void UpdateInternal(float _TimeDelta) override;
  bool ShouldBeUpdated() const override;

private:
  glm::vec3 m_Position;
  glm::vec3 m_Forward;
  glm::vec3 m_Up;

  float m_Yaw;
  float m_Pitch;

  // Input state
  bool      m_MoveForward{false};
  bool      m_MoveBackward{false};
  bool      m_MoveLeft{false};
  bool      m_MoveRight{false};
  float     m_SpeedMultiplier{1.0f};
  glm::vec2 m_MouseDelta{0.0f, 0.0f};
};