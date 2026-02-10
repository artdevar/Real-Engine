#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <variant>

using UniformType = std::variant<GLint, GLuint, GLfloat, glm::mat3, glm::mat4, glm::vec2, glm::vec3, glm::vec4>;

constexpr inline GLuint ATTRIB_LOC_POSITION = 0;
constexpr inline GLuint ATTRIB_LOC_NORMAL = 1;
constexpr inline GLuint ATTRIB_LOC_TEXCOORDS = 2;
constexpr inline GLuint ATTRIB_LOC_TANGENT = 3;
constexpr inline GLuint ATTRIB_LOC_BITANGENT = 4;
constexpr inline GLuint ATTRIB_LOC_DRAW_ID = 5;
constexpr inline GLuint ATTRIB_LOC_TRANSFORM = 10;

constexpr inline GLuint BINDING_DIFF_TEXTURE_BUFFER = 0;
constexpr inline GLuint BINDING_SPEC_TEXTURE_BUFFER = 1;
constexpr inline GLuint BINDING_LIGHTING_BUFFER = 5;

constexpr inline GLenum TEXTURE_SHADOW_MAP_UNIT = GL_TEXTURE20;
constexpr inline GLint TEXTURE_SHADOW_MAP_INDEX = 20;

constexpr inline int MAX_POINT_LIGHTS = 5;

enum EAlphaMode : GLuint
{
  Opaque,
  Mask,
  Blend
};

struct TLightDirectional
{
  alignas(16) glm::vec3 Direction;
  alignas(16) glm::vec3 Ambient;
  alignas(16) glm::vec3 Diffuse;
  alignas(16) glm::vec3 Specular;
};

struct TLightPoint
{
  alignas(16) glm::vec3 Position;
  alignas(16) glm::vec3 Ambient;
  alignas(16) glm::vec3 Diffuse;
  alignas(16) glm::vec3 Specular;

  alignas(4) float Constant;
  alignas(4) float Linear;
  alignas(4) float Quadratic;
};

struct TLightSpot
{
  alignas(16) glm::vec3 Position;
  alignas(16) glm::vec3 Direction;
  alignas(16) glm::vec3 Ambient;
  alignas(16) glm::vec3 Diffuse;
  alignas(16) glm::vec3 Specular;

  alignas(4) float CutOff;
  alignas(4) float OuterCutOff;
  alignas(4) float Constant;
  alignas(4) float Linear;
  alignas(4) float Quadratic;
};

struct TShaderLighting
{
  TLightDirectional LightDirectional;
  TLightSpot LightSpot;
  TLightPoint LightPoints[MAX_POINT_LIGHTS];
  alignas(4) int PointLightsCount;
};

//
