#pragma once

class IInputHandler
{
public:
  virtual ~IInputHandler() = default;

  virtual bool OnMousePressed(int _Button, int _Action, int _Mods)
  {
    return false;
  }

  virtual bool ProcessKeyInput(int _Key, int _Action, int _Mods)
  {
    return false;
  }

  virtual bool ProcessMouseMove(float _X, float _Y)
  {
    return false;
  }
};