#pragma once

#include <glm/mat4x4.hpp>
#include <string>
#include <vector>
#include <bitset>
#include <array>

namespace shared
{

  struct TVertex
  {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
  };

  constexpr inline int CUBEMAP_FACES = 6;

  struct TImage
  {
    unsigned char *Data;

    int Width;
    int Height;
    int Channels;
  };

}
