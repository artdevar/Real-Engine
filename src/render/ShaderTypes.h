#pragma once

#include <glm/glm.hpp>
#include <variant>

using UniformType = std::variant<int, unsigned, float, glm::mat3, glm::mat4, glm::vec2, glm::vec3, glm::vec4>;

constexpr inline unsigned ATTRIB_LOC_POSITION    = 0;
constexpr inline unsigned ATTRIB_LOC_NORMAL      = 1;
constexpr inline unsigned ATTRIB_LOC_TANGENT     = 2;
constexpr inline unsigned ATTRIB_LOC_TEXCOORDS_0 = 3;
constexpr inline unsigned ATTRIB_LOC_TEXCOORDS_1 = 4;
constexpr inline unsigned ATTRIB_LOC_TEXCOORDS_2 = 5;
constexpr inline unsigned ATTRIB_LOC_TEXCOORDS_3 = 6;

constexpr inline unsigned BINDING_LIGHTING_BUFFER = 1;

extern const unsigned TEXTURE_BASIC_COLOR_UNIT;
extern const int      TEXTURE_BASIC_COLOR_INDEX;
extern const unsigned TEXTURE_NORMAL_UNIT;
extern const int      TEXTURE_NORMAL_INDEX;
extern const unsigned TEXTURE_EMISSIVE_UNIT;
extern const int      TEXTURE_EMISSIVE_INDEX;
extern const unsigned TEXTURE_METALLIC_ROUGHNESS_UNIT;
extern const int      TEXTURE_METALLIC_ROUGHNESS_INDEX;
extern const unsigned TEXTURE_OCCLUSION_UNIT;
extern const int      TEXTURE_OCCLUSION_INDEX;
extern const unsigned TEXTURE_DEPTH_MAP_UNIT;
extern const int      TEXTURE_DEPTH_MAP_INDEX;
extern const unsigned TEXTURE_SHADOW_MAP_UNIT;
extern const int      TEXTURE_SHADOW_MAP_INDEX;
extern const unsigned TEXTURE_SKYBOX_UNIT;
extern const int      TEXTURE_SKYBOX_INDEX;
extern const unsigned TEXTURE_IRRADIANCE_MAP_UNIT;
extern const int      TEXTURE_IRRADIANCE_MAP_INDEX;

constexpr inline int MAX_POINT_LIGHTS = 5;

struct TLightDirectional
{
  alignas(16) glm::vec3 Direction;
  alignas(16) glm::vec3 Color;

  alignas(4) float Intensity;
};

struct TShaderLighting
{
  TLightDirectional LightDirectional;
};

//

constexpr float CUBE_VERTICES[] = {-1.0f, 1.0f,  -1.0f, //
                                   -1.0f, -1.0f, -1.0f, //
                                   1.0f,  -1.0f, -1.0f, //
                                   1.0f,  -1.0f, -1.0f, //
                                   1.0f,  1.0f,  -1.0f, //
                                   -1.0f, 1.0f,  -1.0f, //

                                   -1.0f, -1.0f, 1.0f,  //
                                   -1.0f, -1.0f, -1.0f, //
                                   -1.0f, 1.0f,  -1.0f, //
                                   -1.0f, 1.0f,  -1.0f, //
                                   -1.0f, 1.0f,  1.0f,  //
                                   -1.0f, -1.0f, 1.0f,  //

                                   1.0f,  -1.0f, -1.0f, //
                                   1.0f,  -1.0f, 1.0f,  //
                                   1.0f,  1.0f,  1.0f,  //
                                   1.0f,  1.0f,  1.0f,  //
                                   1.0f,  1.0f,  -1.0f, //
                                   1.0f,  -1.0f, -1.0f, //

                                   -1.0f, -1.0f, 1.0f, //
                                   -1.0f, 1.0f,  1.0f, //
                                   1.0f,  1.0f,  1.0f, //
                                   1.0f,  1.0f,  1.0f, //
                                   1.0f,  -1.0f, 1.0f, //
                                   -1.0f, -1.0f, 1.0f, //

                                   -1.0f, 1.0f,  -1.0f, //
                                   1.0f,  1.0f,  -1.0f, //
                                   1.0f,  1.0f,  1.0f,  //
                                   1.0f,  1.0f,  1.0f,  //
                                   -1.0f, 1.0f,  1.0f,  //
                                   -1.0f, 1.0f,  -1.0f, //

                                   -1.0f, -1.0f, -1.0f, //
                                   -1.0f, -1.0f, 1.0f,  //
                                   1.0f,  -1.0f, -1.0f, //
                                   1.0f,  -1.0f, -1.0f, //
                                   -1.0f, -1.0f, 1.0f,  //
                                   1.0f,  -1.0f, 1.0f};

constexpr float QUAD_VERTICES[] = {
    // positions        // texcoords

    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //

    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
};