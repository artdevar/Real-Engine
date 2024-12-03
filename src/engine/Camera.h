#pragma once

#include "interfaces/Updateable.h"
#include "interfaces/InputHandler.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <optional>
#include <set>

class CCamera final :
  public IUpdateable,
  public IInputHandler
{
public:

  CCamera();

  void Update(float _TimeDelta) override;

  bool OnMousePressed(int _Button, int _Action, int _Mods) override;

  bool ProcessKeyInput(int _Key, int _Action, int _Mods) override;

  bool ProcessMouseMove(float _X, float _Y) override;

  void SetPosition(const glm::vec3 & _Pos);

  glm::vec3 GetPosition() const;

  glm::vec3 GetForwardVector() const;

  glm::vec3 GetUpVector() const;

  glm::mat4 GetView() const;

  glm::mat4 GetProjection() const;

  float GetFOV() const;

private:

  glm::vec3 m_Position;
  glm::vec3 m_Forward;
  glm::vec3 m_Up;

  float m_FOV;
  float m_Yaw;
  float m_Pitch;

  std::optional<glm::vec2> m_PrevMousePos;
  std::set<int>            m_PressedKeys;
};