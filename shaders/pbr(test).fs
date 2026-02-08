#version 460 core

const int MAX_POINT_LIGHTS = 5;

struct TLightDirectional
{
  vec3 Direction;
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;
};

struct TLightPoint
{
  vec3 Position;
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;

  float Constant;
  float Linear;
  float Quadratic;
};

struct TLightSpot
{
  vec3 Position;
  vec3 Direction;
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;

  float CutOff;
  float OuterCutOff;
  float Constant;
  float Linear;
  float Quadratic;
};

layout (std140) uniform u_Lighting
{
  TLightDirectional LightDirectional;
  TLightSpot        LightSpot;
  TLightPoint       LightPoints[MAX_POINT_LIGHTS];
  int               PointLightsCount;
};

struct TMaterial
{
  sampler2D BaseColorTexture;
  sampler2D RoughnessTexture;
  sampler2D NormalTexture;

  vec4  BaseColorFactor;
  float MetallicFactor;
  float RoughnessFactor;
  float AlphaCutoff;
  int   AlphaMode;
};

uniform TMaterial u_Material;
uniform sampler2D u_ShadowMap;
uniform mat4      u_LightSpaceMatrix;
uniform vec3      u_ViewPos;

in vec3 io_Normal;
in vec3 io_FragPos;
in vec2 io_TexCoords;
in mat3 io_TBN;
in vec4 io_FragLightPos;

out vec4 o_FragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    float shadow = currentDepth;

    return shadow;
}

void main()
{
    vec4 baseColorSample = texture(u_Material.BaseColorTexture, io_TexCoords) * u_Material.BaseColorFactor;
    if (u_Material.AlphaMode == 2 && baseColorSample.a < u_Material.AlphaCutoff)
      discard;

    vec3 ambientColor = LightDirectional.Ambient * baseColorSample.rgb;

    float shadow = ShadowCalculation(io_FragLightPos);
    ambientColor.r = shadow;
    ambientColor.g = shadow;
    ambientColor.b = shadow;

    o_FragColor = vec4(ambientColor, 1.0);
}
