#version 460 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 io_WorldPos;

void main()
{
  gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
  io_WorldPos = a_Position;
}
