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
  sampler2D EmissiveTexture;

  vec4  BaseColorFactor;
  vec3  EmissiveFactor;
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
in vec4 io_FragLightPos;
in mat3 io_TBN;

out vec4 o_FragColor;

float CalculateShadow(vec4 fragLightPos, vec3 lightDir)
{
    vec3 lightCoords = fragLightPos.xyz / fragLightPos.w;
    if (lightCoords.z > 1.0)
        return 0.0;

    lightCoords = lightCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective
    float closestDepth = texture(u_ShadowMap, lightCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = lightCoords.z;

    //float bias = max(0.05 * (1.0 - dot(io_Normal, lightDir)), 0.005);
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, lightCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    vec4 baseColorSample = texture(u_Material.BaseColorTexture, io_TexCoords) * u_Material.BaseColorFactor;
    if (u_Material.AlphaMode == 2 && baseColorSample.a < u_Material.AlphaCutoff)
        discard;

    // ambient
    vec3 ambient = LightDirectional.Ambient;

    // diffuse
    vec3 norm = texture(u_Material.NormalTexture, io_TexCoords).rgb;
    norm = norm * 2.0 - 1.0;
    norm = normalize(io_TBN * norm);

    vec3 lightDir = normalize(-LightDirectional.Direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightDirectional.Diffuse;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_ViewPos - io_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * LightDirectional.Specular;

    // emmissive
    vec4 emissiveSample = texture(u_Material.EmissiveTexture, io_TexCoords) * vec4(u_Material.EmissiveFactor, 1.0);

    //  shadow
    float shadow = CalculateShadow(io_FragLightPos, lightDir);

    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * baseColorSample.rgb + emissiveSample.rgb;
    o_FragColor = vec4(result, baseColorSample.a);
}
