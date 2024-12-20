#version 450 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in uint aDrawId;
layout (location = 10) in mat4 aModel;

layout (location = 0) flat out uint aoDrawId;
layout (location = 1) out vec2 aoTexCoords;

out vec3 io_Normal;
out vec3 io_FragPos;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
  aoDrawId     = aDrawId;
  aoTexCoords  = aTexCoords;
  io_FragPos   = vec3(aModel * vec4(aPos, 1.0));
  io_Normal    = mat3(aModel) * aNormal;
  gl_Position  = u_Projection * u_View * aModel * vec4(aPos, 1.0);
}