#pragma once

#include "render/Buffer.h"
#include "render/ShaderTypes.h"
#include "render/RenderTypes.h"
#include <common/Common.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <memory>

class CModel;
class CTextureBase;

namespace ecs
{

struct TTransformComponent
{
  glm::mat4x4 WorldMatrix = glm::mat4x4(1.0f);
};

////////////////////////////////////////////////////////////////////

struct TModelComponent
{
  struct TTexture
  {
    std::shared_ptr<CTextureBase> Texture       = nullptr;
    GLint                         TexCoordIndex = 0;
  };

  struct TPrimitiveData
  {
    CVertexArray                  VAO;
    std::vector<CVertexBuffer>    VBOs;
    std::optional<CElementBuffer> EBO;
    EPrimitiveMode                Mode            = EPrimitiveMode::Triangles;
    glm::mat4                     PrimitiveMatrix = glm::mat4(1.0f);
    int                           MaterialIndex   = -1;
    uint32_t                      VerticesCount   = 0;
    uint32_t                      IndicesCount    = 0;
    EIndexType                    Type            = EIndexType::None;
  };

  struct TMaterialData
  {
    TTexture   BaseColorTexture;
    TTexture   MetallicRoughnessTexture;
    TTexture   NormalTexture;
    TTexture   EmissiveTexture;
    glm::vec4  BaseColorFactor = glm::vec4(1.0f);
    glm::vec3  EmissiveFactor  = glm::vec3(0.0f);
    float      MetallicFactor  = 1.0f;
    float      RoughnessFactor = 1.0f;
    float      AlphaCutoff     = 0.5f;
    EAlphaMode AlphaMode       = EAlphaMode::Opaque;
    bool       IsDoubleSided   = false;
  };

  std::vector<TPrimitiveData> Primitives;
  std::vector<TMaterialData>  Materials;
};

////////////////////////////////////////////////////////////////////

struct TLightComponent
{
  ELightType Type;

  glm::vec3 Position  = glm::vec3(0.0f);
  glm::vec3 Direction = glm::vec3(0.0f);
  glm::vec3 Ambient   = glm::vec3(0.0f);
  glm::vec3 Diffuse   = glm::vec3(0.0f);
  glm::vec3 Specular  = glm::vec3(0.0f);

  float Constant    = 1.0f;
  float Linear      = 0.09f;
  float Quadratic   = 0.032f;
  float CutOff      = 0.0f;
  float OuterCutOff = 0.0f;
};

////////////////////////////////////////////////////////////////////

struct TSkyboxComponent
{
  std::shared_ptr<CTextureBase> SkyboxTexture;
  CVertexArray                  VAO;
  CVertexBuffer                 VBO           = GL_STATIC_DRAW;
  uint32_t                      VerticesCount = 0;
};

/////////////////////////////////////////////////////////////////

} // namespace ecs