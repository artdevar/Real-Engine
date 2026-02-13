#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords_0;

out vec3 io_Normal;
out vec3 io_FragPos;
out vec2 io_TexCoords;
out mat3 io_TBN;
out vec4 io_FragLightPos;

uniform mat4 u_Model;
uniform mat4 u_MVP;
uniform mat4 u_LightSpaceMatrix;

void main()
{
  vec3 T = normalize(vec3(u_Model * vec4(aTangent, 0.0)));
  vec3 N = normalize(vec3(u_Model * vec4(aNormal, 0.0)));
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  io_TBN = mat3(T, B, N);
  io_FragPos = vec3(u_Model * vec4(aPos, 1.0));
  io_Normal = mat3(u_Model) * aNormal;
  io_TexCoords = aTexCoords_0;
  io_FragLightPos = u_LightSpaceMatrix * vec4(io_FragPos, 1.0);
  gl_Position = u_MVP * vec4(aPos, 1.0);
}