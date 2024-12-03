#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <memory>
#include "graphics/Buffer.h"
#include "utils/Common.h"
#include "Shared.h"

struct TModel;
class CTextureBase;

enum class ELightType
{
  Directional,
  Point,
  Spotlight
};

namespace ecs
{

struct TTransformComponent
{
  glm::mat4x4 Transform = glm::mat4x4(1.0f);
};

struct TLightComponent
{
  ELightType Type;

  glm::vec3 Position;
  glm::vec3 Direction;
  glm::vec3 Ambient;
  glm::vec3 Diffuse;
  glm::vec3 Specular;

  float Constant    = 1.0f;
  float Linear      = 0.09f;
  float Quadratic   = 0.032f;
  float CutOff      = 0.0f;
  float OuterCutOff = 0.0f;
};

struct TSkyboxComponent
{
  std::shared_ptr<CTextureBase> Cubemap;
};

struct TModelComponent
{
  std::shared_ptr<TModel> Model;
  unsigned int vertices;
  unsigned int indices;
};


}