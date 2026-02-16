#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <memory>
#include <map>
#include <variant>
#include "graphics/Buffer.h"
#include "graphics/ShaderTypes.h"
#include "graphics/RenderTypes.h"
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
    glm::mat4x4 WorldMatrix = glm::mat4x4(1.0f);
  };

  struct TModelComponent
  {
    struct TIndicesData
    {
      uint32_t Indices = 0;
      EIndexType Type = EIndexType::UnsignedInt;
    };

    struct TVerticesData
    {
      uint32_t Vertices = 0;
    };

    struct TTexture
    {
      GLuint Texture = 0;
      GLint TexCoordIndex = 0;
    };

    struct TPrimitiveData
    {
      CVertexArray VAO;
      EPrimitiveMode Mode = EPrimitiveMode::Triangles;
      std::variant<TIndicesData, TVerticesData> DrawData;
      glm::mat4 PrimitiveMatrix = glm::mat4(1.0f);
      int MaterialIndex = -1;
    };

    struct TMaterialData
    {
      TTexture BaseColorTexture;
      TTexture MetallicRoughnessTexture;
      TTexture NormalTexture;
      TTexture EmissiveTexture;
      glm::vec4 BaseColorFactor = glm::vec4(1.0f);
      glm::vec3 EmissiveFactor = glm::vec3(0.0f);
      float MetallicFactor = 1.0f;
      float RoughnessFactor = 1.0f;
      float AlphaCutoff = 0.5f;
      EAlphaMode AlphaMode = EAlphaMode::Opaque;
      bool IsDoubleSided = false;
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
    CVertexArray VAO;
    GLuint SkyboxTexture = 0;
    int VerticesCount = 0;
  };

}