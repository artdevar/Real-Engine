#pragma once

#include <numbers>
#include <limits>
#include <cmath>
#include <glm/gtc/quaternion.hpp>

struct EulerAngles
{
  int Roll;
  int Pitch;
  int Yaw;
};

namespace utils
{

inline bool AreEqual(float _Lhs, float _Rhs)
{
  return fabsf(_Lhs - _Rhs) < std::numeric_limits<float>::epsilon();
}

inline bool IsZero(float _Val)
{
  return AreEqual(_Val, 0.0f);
}

EulerAngles ToEulerAngles(const glm::quat & q)
{
  EulerAngles angles;

  // roll (x-axis rotation)
  const float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
  const float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
  angles.Roll = std::lround(glm::degrees(atan2f(sinr_cosp, cosr_cosp)));

  // pitch (y-axis rotation)
  const float sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
  const float cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
  angles.Pitch = std::lround(glm::degrees(2 * atan2f(sinp, cosp) - std::numbers::pi_v<float> / 2));

  // yaw (z-axis rotation)
  const float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
  const float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
  angles.Yaw = std::lround(glm::degrees(atan2f(siny_cosp, cosy_cosp)));

  return angles;
}

}