#pragma once

#include <numbers>
#include <limits>
#include <cmath>
#include <glm/gtc/quaternion.hpp>

struct EulerAngles
{
  float Roll;
  float Pitch;
  float Yaw;
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

  EulerAngles ToEulerAngles(const glm::quat &q)
  {
    EulerAngles angles;

    // roll (x-axis rotation)
    const float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
    const float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    angles.Roll = glm::degrees(atan2f(sinr_cosp, cosr_cosp));

    // pitch (y-axis rotation)
    const float sinp = 2.0f * (q.w * q.y - q.z * q.x);
    if (sinp >= 1.0f)
      angles.Pitch = glm::degrees(std::numbers::pi_v<float> / 2.0f);
    else if (sinp <= -1.0f)
      angles.Pitch = glm::degrees(-std::numbers::pi_v<float> / 2.0f);
    else
      angles.Pitch = glm::degrees(asinf(sinp));

    // yaw (z-axis rotation)
    const float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
    const float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    angles.Yaw = glm::degrees(atan2f(siny_cosp, cosy_cosp));

    return angles;
  }

}