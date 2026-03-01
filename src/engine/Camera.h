#pragma once

#include "interfaces/InputHandler.h"
#include "interfaces/Updateable.h"
#include "interfaces/Sharable.h"
#include "interfaces/EventsListener.h"
#include "interfaces/Shutdownable.h"

class CCamera final : public IUpdateable,
                      public IInputHandler,
                      public IEventsListener,
                      public IShutdownable,
                      public CSharable<CCamera>
{
public:
  CCamera();

  void Init();

  void Shutdown() override;

  void Update(float _TimeDelta) override;

  void SetPosition(const glm::vec3 &_Pos);
  glm::vec3 GetPosition() const;
  glm::vec3 GetForwardVector() const;
  glm::vec3 GetUpVector() const;
  glm::mat4 GetView() const;
  glm::mat4 GetProjection() const;

  void OnEvent(const TEvent &_Event) override;

public: // Input
  bool OnMousePressed(int _Button, int _Action, int _Mods) override;

  bool ProcessKeyInput(int _Key, int _Action, int _Mods) override;

  bool ProcessMouseMove(float _X, float _Y) override;

  void ResetInputState();

private:
  glm::vec3 m_Position;
  glm::vec3 m_Forward;
  glm::vec3 m_Up;

  float m_Yaw;
  float m_Pitch;

  float m_FOV;
  float m_ZNear;
  float m_ZFar;

  // Input state
  bool      m_MoveForward{false};
  bool      m_MoveBackward{false};
  bool      m_MoveLeft{false};
  bool      m_MoveRight{false};
  float     m_SpeedMultiplier{1.0f};
  glm::vec2 m_MouseDelta{0.0f, 0.0f};
};