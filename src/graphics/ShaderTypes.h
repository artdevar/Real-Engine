#pragma once

#include <glm/glm.hpp>

// Lighting

constexpr inline int MaxPointLights = 5;

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
  TLightSpot        LightSpot;
  TLightPoint       LightPoints[MaxPointLights];
  alignas(4) int    PointLightsCount;
};

//
