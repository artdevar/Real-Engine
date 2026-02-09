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

const float PI = 3.14159265359;

// Fresnel Schlick approximation
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Normal Distribution Function (GGX)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Geometry function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

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

    vec3 albedo = pow(baseColorSample.rgb, vec3(2.2)); // sRGB to linear
    vec3 ambientColor = LightDirectional.Ambient * albedo;

    vec2 mrSample   = texture(u_Material.RoughnessTexture, io_TexCoords).bg;
    float metallic  = clamp(mrSample.r * u_Material.MetallicFactor, 0.0, 1.0);
    float roughness = clamp(mrSample.g * u_Material.RoughnessFactor, 0.05, 1.0); // avoid 0 roughness

    vec3 normal = texture(u_Material.NormalTexture, io_TexCoords).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(io_TBN * normal);

    vec3 N = normalize(normal);
    vec3 V = normalize(u_ViewPos - io_FragPos);
    vec3 L = normalize(-LightDirectional.Direction);
    vec3 H = normalize(V + L);

    // Cook-Torrance BRDF terms
    float NDF = DistributionGGX(N, H, roughness);        // Microfacet distribution
    float G   = GeometrySmith(N, V, L, roughness);       // Geometry (shadowing/masking)
    vec3 F0   = mix(vec3(0.04), albedo, metallic);       // Base reflectivity
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0); // Fresnel term

    // Combine specular BRDF
    float NdotL       = max(dot(N, L), 0.0);
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.001;
    vec3 specular     = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float shadow = CalculateShadow(io_FragLightPos, normalize(-LightDirectional.Direction));
    vec3 color = (kD * albedo / PI + specular) * LightDirectional.Diffuse * NdotL * (1.0 - shadow) + ambientColor;
    //float outAlpha = (u_AlphaMode == 2) ? baseColorSample.a : 1.0;
    o_FragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
