#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 io_Normal;
out vec3 io_FragPos;
out vec2 io_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
  io_FragPos   = vec3(u_Model * vec4(aPos, 1.0));
  io_Normal    = mat3(u_Model) * aNormal;
  io_TexCoords = aTexCoords;
  gl_Position  = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
}