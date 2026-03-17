#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 io_TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
  io_TexCoords = aPos;
  gl_Position  = u_Projection * u_View * vec4(aPos, 1.0);
}