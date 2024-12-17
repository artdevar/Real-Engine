#version 450 core

layout (location = 0) flat in uint aoDrawId;
layout (location = 1) in vec2      aoTexCoords;

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
  sampler2D DiffuseTexture;
  sampler2D SpecularTexture;
  float     Shininess;
};

uniform TMaterial u_Material;
uniform vec3      u_ViewPos;

in vec3 io_Normal;
in vec3 io_FragPos;

out vec4 o_FragColor;

vec3 CalcDirLight(TLightDirectional _Light, vec3 _Normal, vec3 _ViewDir);
vec3 CalcPointLight(TLightPoint _Light, vec3 _Normal, vec3 _ViewDir);
vec3 CalcSpotLight(TLightSpot _Light, vec3 _Normal, vec3 _ViewDir);

void main()
{
  o_FragColor = vec4(io_Normal, 0.0);
  o_FragColor = vec4(io_FragPos, 0.0);
  o_FragColor = vec4(0.0);

  vec3 normal  = normalize(io_Normal);
  vec3 viewDir = normalize(u_ViewPos - io_FragPos);

  vec3 fragColor = CalcDirLight(LightDirectional, normal, viewDir);
       fragColor += CalcSpotLight(LightSpot, normal, viewDir);

  for(int i = 0; i < PointLightsCount; i++)
      fragColor += CalcPointLight(LightPoints[i], normal, viewDir);

  o_FragColor = vec4(fragColor, 1.0);
  o_FragColor = vec4(1.0);
}

vec3 CalcDirLight(TLightDirectional _Light, vec3 _Normal, vec3 _ViewDir)
{
  vec3 lightDir = normalize(-_Light.Direction);

  float diff = max(dot(_Normal, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir, _Normal);
  float spec = pow(max(dot(_ViewDir, reflectDir), 0.0), u_Material.Shininess);

  vec3 ambient  = _Light.Ambient * texture(u_Material.DiffuseTexture, aoTexCoords).rgb;
  vec3 diffuse  = _Light.Diffuse * diff * texture(u_Material.DiffuseTexture, aoTexCoords).rgb;
  vec3 specular = _Light.Specular * spec * texture(u_Material.SpecularTexture, aoTexCoords).rgb;

  return max(vec3(0.0f), (ambient + diffuse + specular));
}

vec3 CalcPointLight(TLightPoint _Light, vec3 _Normal, vec3 _ViewDir)
{
    vec3 lightDir = normalize(_Light.Position - io_FragPos);

    float diff = max(dot(_Normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, _Normal);
    float spec = pow(max(dot(_ViewDir, reflectDir), 0.0), u_Material.Shininess);

    float distance = length(_Light.Position - io_FragPos);
    float attenuation = 1.0 / (_Light.Constant + _Light.Linear * distance + _Light.Quadratic * (distance * distance));

    vec3 ambient = _Light.Ambient * texture(u_Material.DiffuseTexture, aoTexCoords).rgb;
    vec3 diffuse = _Light.Diffuse * diff * texture(u_Material.DiffuseTexture, aoTexCoords).rgb;
    vec3 specular = _Light.Specular * spec * texture(u_Material.SpecularTexture, aoTexCoords).rgb;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return max(vec3(0.0f), (ambient + diffuse + specular));
}

vec3 CalcSpotLight(TLightSpot _Light, vec3 _Normal, vec3 _ViewDir)
{
    vec3 lightDir = normalize(_Light.Position - io_FragPos);

    float diff = max(dot(_Normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, _Normal);
    float spec = pow(max(dot(_ViewDir, reflectDir), 0.0), u_Material.Shininess);

    float distance = length(_Light.Position - io_FragPos);
    float attenuation = 1.0 / (_Light.Constant + _Light.Linear * distance + _Light.Quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-_Light.Direction));
    float epsilon = _Light.CutOff - _Light.OuterCutOff;
    float intensity = clamp((theta - _Light.OuterCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = _Light.Ambient * texture(u_Material.DiffuseTexture, aoTexCoords).rgb;
    vec3 diffuse = _Light.Diffuse * diff * texture(u_Material.DiffuseTexture, aoTexCoords).rgb;
    vec3 specular = _Light.Specular * spec * texture(u_Material.SpecularTexture, aoTexCoords).rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return max(vec3(0.0f), (ambient + diffuse + specular));
}
