#version 460 core

struct TLightDirectional
{
  vec3 Direction;
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;
};

layout(std140, binding = 1) uniform u_Lighting
{
  TLightDirectional LightDirectional;
};

struct TMaterial
{
  sampler2D BaseColorTexture;
  int       BaseColorTextureTexCoordIndex;

  sampler2D MetallicRoughnessTexture;
  int       MetallicRoughnessTextureTexCoordIndex;

  sampler2D NormalTexture;
  int       NormalTextureTexCoordIndex;

  sampler2D EmissiveTexture;
  int       EmissiveTextureTexCoordIndex;

  vec4  BaseColorFactor;
  vec3  EmissiveFactor;
  float MetallicFactor;
  float RoughnessFactor;
  float AlphaCutoff;
  int   AlphaMode;
  bool  IsDoubleSided;
};

uniform TMaterial u_Material;
uniform sampler2D u_ShadowMap;
uniform mat4      u_LightSpaceMatrix;
uniform vec3      u_ViewPos;

in vec3 io_Normal;
in vec3 io_FragPos;
in vec4 io_FragLightPos;
in mat3 io_TBN;
in vec2 io_TexCoords[4];

out vec4 o_FragColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
  float a  = roughness * roughness;
  float a2 = a * a;

  float NdotH  = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom       = PI * denom * denom;

  return a2 / max(denom, 0.000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
  float r = roughness + 1.0;
  float k = (r * r) / 8.0;

  float denom = NdotV * (1.0 - k) + k;
  return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx1  = GeometrySchlickGGX(NdotV, roughness);
  float ggx2  = GeometrySchlickGGX(NdotL, roughness);
  return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float CalculateShadow(vec4 fragLightPos, vec3 lightDir)
{
  float shadow      = 0.0;
  vec3  lightCoords = fragLightPos.xyz / fragLightPos.w;
  if (lightCoords.z <= 1.0)
  {
    lightCoords = lightCoords * 0.5 + 0.5;

    float closestDepth = texture(u_ShadowMap, lightCoords.xy).r;
    float currentDepth = lightCoords.z;

    shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
      for (int y = -1; y <= 1; ++y)
      {
        float pcfDepth = texture(u_ShadowMap, lightCoords.xy + vec2(x, y) * texelSize).r;
        shadow        += currentDepth > pcfDepth ? 1.0 : 0.0;
      }
    }
    shadow /= 9.0;
  }

  return shadow;
}

void main()
{
  vec2 baseColorTexCoords         = io_TexCoords[u_Material.BaseColorTextureTexCoordIndex];
  vec2 normalTexCoords            = io_TexCoords[u_Material.NormalTextureTexCoordIndex];
  vec2 emissiveTexCoords          = io_TexCoords[u_Material.EmissiveTextureTexCoordIndex];
  vec2 metallicRoughnessTexCoords = io_TexCoords[u_Material.MetallicRoughnessTextureTexCoordIndex];

  // === Base Color ===
  vec4 baseColorSample = texture(u_Material.BaseColorTexture, baseColorTexCoords) * u_Material.BaseColorFactor;

  if (u_Material.AlphaMode == 1 && baseColorSample.a < u_Material.AlphaCutoff)
    discard;

  vec3 albedo = baseColorSample.rgb;

  // === Metallic & Roughness ===
  vec4 mrSample = texture(u_Material.MetallicRoughnessTexture, metallicRoughnessTexCoords);

  float metallic  = mrSample.b * u_Material.MetallicFactor;
  float roughness = mrSample.g * u_Material.RoughnessFactor;
  roughness       = clamp(roughness, 0.04, 1.0);

  // === Normal Mapping ===
  vec3 N = texture(u_Material.NormalTexture, normalTexCoords).rgb;
  N      = normalize(io_TBN * (N * 2.0 - 1.0));

  if (u_Material.IsDoubleSided && !gl_FrontFacing)
    N = -N;

  vec3 V = normalize(u_ViewPos - io_FragPos);
  vec3 L = normalize(-LightDirectional.Direction);
  vec3 H = normalize(V + L);

  float NdotL = max(dot(N, L), 0.0);

  // === Fresnel base reflectance ===
  vec3 F0 = vec3(0.04);
  F0      = mix(F0, albedo, metallic);

  // === Cook-Torrance BRDF ===
  float NDF = DistributionGGX(N, H, roughness);
  float G   = GeometrySmith(N, V, L, roughness);
  vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

  vec3  numerator = NDF * G * F;
  float denom     = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
  vec3  specular  = numerator / denom;

  // Energy conservation
  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD     *= 1.0 - metallic;

  vec3 diffuse = kD * albedo / PI;

  // Shadow
  float shadow = CalculateShadow(io_FragLightPos, L);

  vec3 radiance = LightDirectional.Diffuse;

  vec3 lighting = (1.0 - shadow) * (diffuse + specular) * radiance * NdotL;

  // Ambient (very basic, no IBL yet)
  vec3 ambient = LightDirectional.Ambient * albedo;

  // Emissive
  vec3 emissive = texture(u_Material.EmissiveTexture, emissiveTexCoords).rgb * u_Material.EmissiveFactor;

  vec3  color = ambient + lighting + emissive;
  float alpha = (u_Material.AlphaMode == 0) ? 1.0 : baseColorSample.a;

  o_FragColor = vec4(color, alpha);
}
