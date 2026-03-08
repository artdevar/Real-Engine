#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 io_WorldPos;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
  io_WorldPos = (u_Model * vec4(aPos, 1.0)).xyz;
  gl_Position = u_MVP * vec4(aPos, 1.0);
}
