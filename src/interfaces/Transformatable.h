#pragma once

#include "glm/mat4x4.hpp"

class CTransformable
{
public:

  CTransformable() : m_Transform(1.0f)
  {}

  virtual ~CTransformable() = default;

  inline glm::mat4 GetTransform() const
  {
    return m_Transform;
  }

  inline void SetTransform(const glm::mat4 & _Transform)
  {
    m_Transform = _Transform;
  }

protected:

  glm::mat4 m_Transform;
};