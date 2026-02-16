#version 460 core

struct TLightDirectional
{
  vec3 Direction;
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;
};

layout (std140) uniform u_Lighting
{
  TLightDirectional LightDirectional;
};

struct TMaterial
{
  sampler2D BaseColorTexture;
  int BaseColorTextureTexCoordIndex;

  sampler2D MetallicRoughnessTexture;
  int MetallicRoughnessTextureTexCoordIndex;

  sampler2D NormalTexture;
  int NormalTextureTexCoordIndex;

  sampler2D EmissiveTexture;
  int EmissiveTextureTexCoordIndex;

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

float CalculateShadow(vec4 fragLightPos, vec3 lightDir)
{
    float shadow = 0.0;
    vec3 lightCoords = fragLightPos.xyz / fragLightPos.w;
    if (lightCoords.z <= 1.0)
    {
      lightCoords = lightCoords * 0.5 + 0.5;

      float closestDepth = texture(u_ShadowMap, lightCoords.xy).r;
      float currentDepth = lightCoords.z;

      shadow = currentDepth > closestDepth ? 1.0 : 0.0;

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
    }

    return shadow;
}

void main()
{
    vec2 baseColorTexCoords = io_TexCoords[u_Material.BaseColorTextureTexCoordIndex];
    vec2 normalTexCoords = io_TexCoords[u_Material.NormalTextureTexCoordIndex];
    vec2 emissiveTexCoords = io_TexCoords[u_Material.EmissiveTextureTexCoordIndex];
    vec2 metallicRoughnessTexCoords = io_TexCoords[u_Material.MetallicRoughnessTextureTexCoordIndex];

    vec4 baseColorSample = texture(u_Material.BaseColorTexture, baseColorTexCoords) * u_Material.BaseColorFactor;
    if (u_Material.AlphaMode == 1 && baseColorSample.a < u_Material.AlphaCutoff)
        discard;

    vec3 normal = texture(u_Material.NormalTexture, normalTexCoords).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(io_TBN * normal);

    if (u_Material.IsDoubleSided && !gl_FrontFacing)
      normal = -normal;

    vec3 lightDir = normalize(-LightDirectional.Direction);
    vec3 viewDir = normalize(u_ViewPos - io_FragPos);


    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * LightDirectional.Diffuse;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = LightDirectional.Specular * spec;

    // emmissive
    vec4 emissiveSample = texture(u_Material.EmissiveTexture, emissiveTexCoords) * vec4(u_Material.EmissiveFactor, 1.0);

    // ambient
    vec3 ambient = LightDirectional.Ambient;

    //  shadow
    float shadow = CalculateShadow(io_FragLightPos, lightDir);

    // final color
    float alpha = u_Material.AlphaMode == 0 ? 1.0 : baseColorSample.a;
    vec3 color = (ambient + (1.0 - shadow) * (diffuse + specular)) * baseColorSample.rgb + emissiveSample.rgb;

    o_FragColor = vec4(color, alpha);
}
