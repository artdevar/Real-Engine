#pragma once

#include <glm/vec3.hpp>
#include <glm/ext.hpp>
#include <cfloat>

struct TAABB
{
  glm::vec3 Min = glm::vec3(FLT_MAX);
  glm::vec3 Max = glm::vec3(-FLT_MAX);

  void Expand(const glm::vec3 &_Point)
  {
    Min = glm::min(Min, _Point);
    Max = glm::max(Max, _Point);
  }

  glm::vec3 Center() const
  {
    return (Min + Max) * 0.5f;
  }

  glm::vec3 Size() const
  {
    return Max - Min;
  }

  std::vector<glm::vec3> GetCorners() const
  {
    return {{Min.x, Min.y, Min.z}, {Max.x, Min.y, Min.z}, {Max.x, Max.y, Min.z}, {Min.x, Max.y, Min.z},
            {Min.x, Min.y, Max.z}, {Max.x, Min.y, Max.z}, {Max.x, Max.y, Max.z}, {Min.x, Max.y, Max.z}};
  }
};