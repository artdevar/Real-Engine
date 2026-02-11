#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <memory>
#include <map>
#include "graphics/Buffer.h"
#include "graphics/ShaderTypes.h"
#include "utils/Common.h"

class CModel;
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

  struct TModelComponent
  {
    struct TPrimitiveData
    {
      CVertexArray VAO;
      uint32_t Indices = 0;
      uint32_t Offset = 0;
      int MaterialIndex = -1;
    };

    struct TMaterialData
    {
      std::shared_ptr<CTextureBase> BaseColorTexture;
      std::shared_ptr<CTextureBase> MetallicRoughnessTexture;
      std::shared_ptr<CTextureBase> NormalTexture;
      std::shared_ptr<CTextureBase> EmissiveTexture;
      glm::vec4 BaseColorFactor = glm::vec4(1.0f);
      glm::vec3 EmissiveFactor = glm::vec3(0.0f);
      float MetallicFactor = 1.0f;
      float RoughnessFactor = 1.0f;
      float AlphaCutoff = 0.5f;
      EAlphaMode AlphaMode = EAlphaMode::Opaque;
    };

    std::vector<TPrimitiveData> Primitives;
    std::vector<TMaterialData> Materials;
  };

  struct TLightComponent
  {
    ELightType Type;

    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Direction = glm::vec3(0.0f);
    glm::vec3 Ambient = glm::vec3(0.0f);
    glm::vec3 Diffuse = glm::vec3(0.0f);
    glm::vec3 Specular = glm::vec3(0.0f);

    float Constant = 1.0f;
    float Linear = 0.09f;
    float Quadratic = 0.032f;
    float CutOff = 0.0f;
    float OuterCutOff = 0.0f;
  };

  struct TSkyboxComponent
  {
    std::shared_ptr<CTextureBase> SkyboxTexture;
  };

}